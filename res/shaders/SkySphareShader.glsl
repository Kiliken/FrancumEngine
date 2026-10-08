#version 460 core

#ifdef VERT

layout(location = 0) in vec3 aPos;

layout(location = 1) out vec3 TexCoordDir;

layout(std140, binding = 2) uniform Camera {
    mat4 V;
    mat4 P;
};

void VSMain()
{
    mat4 rotView = mat4(mat3(V));
    TexCoordDir = aPos;
    gl_Position = P * rotView * vec4(aPos, 1.0);
}

#endif

#ifdef FRAG

layout(location = 1) in vec3 TexCoordDir;
layout(location = 21) out vec4 FragColor;

layout(binding = 0) uniform sampler2D skyTex;
layout(location = 25) uniform float iTime;

layout(std140, binding = 3) uniform Light {
    vec3 light_dir_ws;
    float light_power;
    float ambient_light;
};

// Shader Tuning Parameters
const float cloudscale  = 1.1;
const float speed       = 0.03;
const float clouddark   = 0.5;
const float cloudlight  = 0.3;
const float cloudcover  = 0.2;
const float cloudalpha  = 8.0;
const float skytint     = 0.5;

const mat2 m = mat2(1.6, 1.2, -1.2, 1.6);

// --- Fast 2D Simplex Noise ---
vec2 hash(vec2 p) {
    p = vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)));
    return -1.0 + 2.0 * fract(sin(p) * 43758.5453123);
}

float noise(vec2 p) {
    const float K1 = 0.366025404; 
    const float K2 = 0.211324865; 
    vec2 i = floor(p + vec2((p.x + p.y) * K1));  
    vec2 a = p - i + vec2((i.x + i.y) * K2);
    vec2 o = (a.x > a.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0); 
    vec2 b = a - o + vec2(K2);
    vec2 c = a - vec2(1.0) + vec2(2.0 * K2);
    vec3 h = max(0.5 - vec3(dot(a, a), dot(b, b), dot(c, c)), 0.0);
    vec3 n = h * h * h * h * vec3(dot(a, hash(i)), dot(b, hash(i + o)), dot(c, hash(i + vec2(1.0))));
    return dot(n, vec3(70.0));   
}

float fbm(vec2 n) {
    float total = 0.0;
    float amplitude = 0.1;
    for (int i = 0; i < 6; i++) {
        total += noise(n) * amplitude;
        n = m * n;
        amplitude *= 0.4;
    }
    return total;
}

void PSMain()
{
    vec3 dir = normalize(TexCoordDir);
    float time = iTime * speed;

    // Safety fallback for light direction
    vec3 safeLightDir = (length(light_dir_ws) > 0.001) ? light_dir_ws : vec3(0.0, -1.0, 0.0);
    vec3 sunDir = normalize(-safeLightDir);
    float sunDot = dot(dir, sunDir);

    // Atmospheric Sky Gradient & Sun
    float sunGlow = pow(max(sunDot, 0.0), 32.0);
    float sunDisk = smoothstep(cos(radians(0.5)), cos(radians(0.15)), sunDot);
    vec3 sunColor = vec3(1.0, 0.75, 0.45);

    float skyHeight = clamp(dir.y, 0.0, 1.0);
    vec3 horizonColor = vec3(0.55, 0.70, 0.85);
    vec3 zenithColor  = vec3(0.08, 0.25, 0.55);
    vec3 sky = mix(horizonColor, zenithColor, smoothstep(0.0, 1.0, skyHeight));

    // Sunset glow
    float sunset = 1.0 - smoothstep(-0.1, 0.3, sunDir.y);
    sky += vec3(1.0, 0.25, 0.08) * sunset * pow(1.0 - skyHeight, 3.0) * 0.35;

    // ---------------------------------------------------------
    // MULTI-LAYERED CLOUD GENERATION
    // ---------------------------------------------------------
    vec3 finalCloudColor = vec3(0.0);
    float cloudFactor = 0.0;

    if (dir.y > 0.0) 
    {
        // Planar Projection onto a sky plane
        float safeY = max(dir.y, 0.03);
        vec2 p = dir.xz / safeY;
        
        // Base coordinate warping offset
        float q = fbm(p * cloudscale * 0.5);

        // Ridged Noise Shape
        float r = 0.0;
        vec2 uv = p * cloudscale - vec2(q - time);
        float weight = 0.8;
        for (int i = 0; i < 6; i++) {
            r += abs(weight * noise(uv));
            uv = m * uv + vec2(time);
            weight *= 0.7;
        }

        // Smooth Noise Shape
        float f = 0.0;
        uv = p * cloudscale - vec2(q - time);
        weight = 0.7;
        for (int i = 0; i < 6; i++) {
            f += weight * noise(uv);
            uv = m * uv + vec2(time);
            weight *= 0.6;
        }

        f *= (r + f);

        // Color Shading Noise
        float c = 0.0;
        float timeColor = iTime * speed * 2.0;
        uv = p * cloudscale * 2.0 - vec2(q - timeColor);
        weight = 0.4;
        for (int i = 0; i < 6; i++) {
            c += weight * noise(uv);
            uv = m * uv + vec2(timeColor);
            weight *= 0.6;
        }

        // Ridge Color Detail
        float c1 = 0.0;
        float timeRidge = iTime * speed * 3.0;
        uv = p * cloudscale * 3.0 - vec2(q - timeRidge);
        weight = 0.4;
        for (int i = 0; i < 6; i++) {
            c1 += abs(weight * noise(uv));
            uv = m * uv + vec2(timeRidge);
            weight *= 0.6;
        }

        c += c1;

        // Compute Base Cloud Color and Density
        vec3 rawCloudColor = vec3(1.1, 1.1, 0.9) * clamp(clouddark + cloudlight * c, 0.0, 1.0);
        
        // Sun lighting contribution
        float sunLight = max(sunDot, 0.0);
        rawCloudColor += sunColor * pow(sunLight, 6.0) * 0.4;

        f = cloudcover + cloudalpha * f * r;

        // Combined blend opacity
        cloudFactor = clamp(f + c, 0.0, 1.0);

        // Fade out clouds as they approach the horizon
        float horizonFade = smoothstep(0.02, 0.18, dir.y);
        cloudFactor *= horizonFade;

        finalCloudColor = clamp(skytint * sky + rawCloudColor, 0.0, 1.0);
    }

    // ---------------------------------------------------------
    // COMBINE SKY, CLOUDS, AND SUN
    // ---------------------------------------------------------
    vec3 result = mix(sky, finalCloudColor, cloudFactor);

    // Add Sun Disk and Atmospheric Glow
    result += sunColor * sunGlow * 0.5;
    result += sunColor * sunDisk * 4.0;

    // Final smooth horizon clamp
    float horizonMask = smoothstep(-0.05, 0.15, dir.y);
    result = mix(sky, result, horizonMask);

    FragColor = vec4(result, 1.0);
}

#endif