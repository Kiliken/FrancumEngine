#pragma once

#include <GL/glew.h>
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

Inputs::Inputs(GLFWwindow *mainWindow)
{
    win = mainWindow;
}

Inputs::~Inputs()
{
}

void Inputs::Update(float dt)
{
}

bool Inputs::IsKeyDown(int key)
{
    return glfwGetKey(win, key) == GLFW_PRESS;
}