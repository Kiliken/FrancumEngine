#include "Inputs.h"

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