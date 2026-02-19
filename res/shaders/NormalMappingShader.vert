#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;
layout(location = 3) in vec3 vertexTangent_modelspace;
layout(location = 4) in vec3 vertexBitangent_modelspace;

out vec2 UV;
out vec3 Position_worldspace;
out vec3 EyeDirection_cameraspace;

out vec3 LightDirection_tangentspace;
out vec3 EyeDirection_tangentspace;

uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform mat3 MV3x3;
uniform vec3 LightDirection_worldspace;

void main() {

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
