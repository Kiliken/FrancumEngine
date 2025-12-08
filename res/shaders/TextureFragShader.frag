#version 440 core

// Interpolated values from the vertex shaders
in vec2 UV;

// Ouput data
out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;

void main(){

    // Fix UVs form DDS(DirectX) standard to GL
    vec2 fixedUV = vec2(UV.x, 1.0 - UV.y);

    // Output color = color of the texture at the specified UV
    color = texture( myTextureSampler, fixedUV ).rgb;
}