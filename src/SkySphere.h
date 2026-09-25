#pragma once

#include <iostream>
#include <glad/gl.h>
#include <SDL3/SDL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "utils/loadShader.h"
#include "utils/loadDDS.h"
#include "Inputs.h"
#include "utils/vboIndexer.h"

class SkySphere
{

public:
    SkySphere();
    ~SkySphere();

    void Draw();

private:
    // Render

    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;

    const int X_SEG = 64;
    const int Y_SEG = 64;
    const float PI = glm::pi<float>();

    // Shader Parm
    GLuint shaders;

    GLuint vao;
    GLuint vertexBuffer;
    GLuint indexBuffer;

    GLuint Texture;
    GLuint textureID;
};