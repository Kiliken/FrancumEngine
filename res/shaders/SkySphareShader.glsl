#version 460 core

#ifdef VERT

layout(location = 0) in vec3 aPos;

layout(location = 1) out vec3 TexCoordDir;

layout(location = 10) uniform mat4 view;
layout(location = 11) uniform mat4 projection;

void VSMain()
{
    // Remove translation from view matrix
    mat4 rotView = mat4(mat3(view));
	TexCoordDir = aPos;
	
    gl_Position = projection * rotView * vec4(aPos, 1.0);
}

#endif

#ifdef FRAG

layout(location = 1) in vec3 TexCoordDir;

layout(location = 21) out vec4 FragColor;

layout(location = 12) uniform sampler2D skyTex;



void PSMain()
{
    // Convert direction to spherical UV
    float u = atan(TexCoordDir.z, TexCoordDir.x) / (2.0 * 3.1415926) + 0.5;
    float v = TexCoordDir.y * 0.5 + 0.5;

    FragColor = texture(skyTex, vec2(u, v));
}

#endif
