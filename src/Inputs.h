#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Inputs
{
private:
    // window reference
    GLFWwindow *win;



public:
    Inputs(GLFWwindow *mainWindow);
    ~Inputs();
    void Update(float dt);

    bool IsKeyDown(int key);

};