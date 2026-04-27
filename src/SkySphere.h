#pragma once

#include <iostream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "loadShader.h"
#include "loadDDS.h"
#include "Inputs.h"
#include "vboIndexer.h"

class SkySphere
{

public:
    SkySphere(glm::mat4* proj, glm::mat4* View);
    ~SkySphere();

    void Draw();

private:
    // Render

    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;

    const int X_SEG = 64;
    const int Y_SEG = 64;

    // Shader Parm
    GLuint shaders;

    GLuint vao;
    GLuint vertexBuffer;
    GLuint indexBuffer;

    GLuint ProjID;
    GLuint ViewID;

    GLuint Texture;
    GLuint textureID;

    // Transform
    glm::mat4 *projection;
    glm::mat4 *view;
};

SkySphere::SkySphere(glm::mat4* proj, glm::mat4* View)
    : projection(proj), view(View)
{
    // Generate Vertices
    for (int y = 0; y <= Y_SEG; y++)
    {
        for (int x = 0; x <= X_SEG; x++)
        {

            float xSeg = (float)x / (float)X_SEG;
            float ySeg = (float)y / (float)Y_SEG;

            float xPos = std::cos(xSeg * 2.0f * M_PI) * std::sin(ySeg * M_PI);
            float yPos = std::cos(ySeg * M_PI);
            float zPos = std::sin(xSeg * 2.0f * M_PI) * std::sin(ySeg * M_PI);

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

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    glBindVertexArray(0);


    ProjID = glGetUniformLocation(shaders, "projection");
    ViewID = glGetUniformLocation(shaders, "view");
    
    Texture = loadDDS("../res/baseDiffuse.dds");
    textureID = glGetUniformLocation(shaders, "skyTex");

}

void SkySphere::Draw()
{
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);

    glUseProgram(shaders);

    glBindVertexArray(vao);

    glUniformMatrix4fv(ViewID, 1, GL_FALSE, &((*view)[0][0]));
    glUniformMatrix4fv(ProjID, 1, GL_FALSE, &((*projection)[0][0]));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture);
    glUniform1i(textureID, 0);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
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