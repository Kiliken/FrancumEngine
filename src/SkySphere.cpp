#include "SkySphere.h"

#include "configs.h"

SkySphere::SkySphere()
{
    // Generate Vertices
    for (int y = 0; y <= Y_SEG; y++)
    {
        for (int x = 0; x <= X_SEG; x++)
        {

            float xSeg = (float)x / (float)X_SEG;
            float ySeg = (float)y / (float)Y_SEG;

            float xPos = std::cos(xSeg * 2.0f * PI) * std::sin(ySeg * PI);
            float yPos = std::cos(ySeg * PI);
            float zPos = std::sin(xSeg * 2.0f * PI) * std::sin(ySeg * PI);

            vertices.push_back({xPos, yPos, zPos});
        }
    }

    // Generate indices
    for (int y = 0; y < Y_SEG; y++)
    {
        for (int x = 0; x < X_SEG; x++)
        {

            int i0 = y * (X_SEG + 1) + x;
            int i1 = (y + 1) * (X_SEG + 1) + x;
            int i2 = (y + 1) * (X_SEG + 1) + (x + 1);
            int i3 = y * (X_SEG + 1) + (x + 1);

            indices.push_back(i0);
            indices.push_back(i1);
            indices.push_back(i2);

            indices.push_back(i0);
            indices.push_back(i2);
            indices.push_back(i3);
        }
    }

    shaders = Utils::LoadSPIRV(ASSETS("shaders/SkySphareShader.vert.spv"), ASSETS("shaders/SkySphareShader.frag.spv"));

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    glBindVertexArray(0);
    
    Texture = Utils::loadDDS(ASSETS("models/skybox.dds"));
    textureID = glGetUniformLocation(shaders, "skyTex");

}

void SkySphere::Draw()
{
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);

    glUseProgram(shaders);


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture);

    if (textureID != -1) {
        glUniform1i(textureID, 0);
    }

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
}

SkySphere::~SkySphere()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &indexBuffer);
    glDeleteProgram(shaders);
}