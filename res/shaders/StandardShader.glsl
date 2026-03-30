#version 460 core

#ifdef VERT

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;
layout(location = 3) in vec3 vertexTangent_modelspace;
layout(location = 4) in vec3 vertexBitangent_modelspace;

layout(location = 20) out vec2 UV;
layout(location = 21) out vec3 Position_worldspace;
layout(location = 22) out vec3 EyeDirection_cameraspace;

layout(location = 23) out vec3 LightDirection_tangentspace;
layout(location = 24) out vec3 EyeDirection_tangentspace;

layout(location = 10) uniform mat4 MVP;
layout(location = 11) uniform mat4 V;
layout(location = 12) uniform mat4 M;
layout(location = 13) uniform mat3 MV3x3;
layout(location = 14) uniform vec3 LightDirection_worldspace;


void VSMain() {

    gl_Position = MVP * vec4(vertexPosition_modelspace, 1.0);

    // World-space position
    Position_worldspace = (M * vec4(vertexPosition_modelspace, 1.0)).xyz;

    // Camera-space position
    vec3 vertexPosition_cameraspace = (V * vec4(Position_worldspace, 1.0)).xyz;

    // Eye direction (camera is at origin in camera space)
    EyeDirection_cameraspace = -vertexPosition_cameraspace;

    // Convert light direction to camera space
    vec3 LightDirection_cameraspace =
        mat3(V) * (-LightDirection_worldspace); 

    // Build TBN in camera space
    vec3 T = normalize(MV3x3 * vertexTangent_modelspace);
    vec3 B = normalize(MV3x3 * vertexBitangent_modelspace);
    vec3 N = normalize(MV3x3 * vertexNormal_modelspace);

    mat3 TBN = transpose(mat3(T, B, N));

    // Convert to tangent space
    LightDirection_tangentspace = TBN * LightDirection_cameraspace;
    EyeDirection_tangentspace   = TBN * EyeDirection_cameraspace;

    UV = vertexUV;
}

#endif

#ifdef FRAG

layout(location = 20) in vec2 UV;
layout(location = 23) in vec3 LightDirection_tangentspace;
layout(location = 24) in vec3 EyeDirection_tangentspace;

layout(location = 25) out vec3 color;

layout(location = 15) uniform sampler2D DiffuseTextureSampler;
layout(location = 16) uniform sampler2D NormalTextureSampler;
layout(location = 17) uniform sampler2D SpecularTextureSampler;


layout(location = 18) uniform float AmbientStrength = 0.2;
layout(location = 19) uniform float LightPower = 1.1;


void PSMain() {

    // Material
    vec3 albedo = texture(DiffuseTextureSampler, UV).rgb;
    vec3 specMap = texture(SpecularTextureSampler, UV).rgb * 0.3;

    // Normal map
    vec3 normal = texture(NormalTextureSampler, UV).rgb * 2.0 - 1.0;
    normal = normalize(normal);

    // Lights
    vec3 L = normalize(LightDirection_tangentspace);
    vec3 E = normalize(EyeDirection_tangentspace);

    // Half-Lambert
    float NdotL = dot(normal, L);
    float halfLambert = NdotL * 0.5 + 0.5;   // shifts [-1..1] → [0..1]
    halfLambert = clamp(halfLambert, 0.0, 1.0);

    // Specular
    vec3 R = reflect(-L, normal);
    float spec = pow(max(dot(E, R), 0.0), 16.0);

    // Ambient
    vec3 ambient = AmbientStrength * albedo;

    // Color
    color = ambient +
            albedo * halfLambert * LightPower +
            specMap * spec * LightPower;

    // Gamma Correction
    color = pow(color, vec3(1.0 / 2.2));
}

#endif