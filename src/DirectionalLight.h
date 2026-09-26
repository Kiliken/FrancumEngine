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

class DirectionalLight
{
public:
    DirectionalLight();
    ~DirectionalLight();

    void BindToShader(const GLuint& shader);
    void Update();

    float yaw;   // Rotation around Y-axis
    float pitch;  // Tilt down towards scene
    glm::vec3 pos;
    GLuint glID;

private:
    

};
