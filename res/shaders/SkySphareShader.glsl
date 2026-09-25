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
    // Remove translation from view matrix
    mat4 rotView = mat4(mat3(V));
	TexCoordDir = aPos;
	
    gl_Position = P * rotView * vec4(aPos, 1.0);
}

#endif

#ifdef FRAG

layout(location = 1) in vec3 TexCoordDir;

layout(location = 21) out vec4 FragColor;

layout(location = 12) uniform sampler2D skyTex;

const float PI = 3.14159265359;

void PSMain()
{
    vec3 dir = normalize(TexCoordDir);

    float u = atan(dir.z, dir.x) / (2.0 * PI) + 0.5;
    float v = 0.5 - (asin(clamp(dir.y, -1.0, 1.0)) / PI);

    FragColor = texture(skyTex, vec2(u, v));
}

#endif
