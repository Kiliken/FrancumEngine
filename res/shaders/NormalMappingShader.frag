#version 460 core

layout(location = 20) in vec2 UV;
layout(location = 23) in vec3 LightDirection_tangentspace;
layout(location = 24) in vec3 EyeDirection_tangentspace;

layout(location = 25) out vec3 color;

layout(location = 15) uniform sampler2D DiffuseTextureSampler;
layout(location = 16) uniform sampler2D NormalTextureSampler;
layout(location = 17) uniform sampler2D SpecularTextureSampler;


layout(location = 18) uniform float AmbientStrength = 0.2;
layout(location = 19) uniform float LightPower = 1.1;

void main() {

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
