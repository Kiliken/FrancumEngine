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

    // win size
    int winWidth, winHeight;

    // mouse Pos
    double xpos, ypos;
    // mouse Scroll
    double scrollY = 0.0;
    // horizontal angle : toward -Z
    float horizontalAngle = 3.14f;
    // vertical angle : 0, look at the horizon
    float verticalAngle = 0.0f;
    // Initial Field of View
    float initialFoV = 45.0f;

    float speed = 3.0f;
    float mouseSpeed = 0.05f;

    bool tabWasDown;

public:
    Inputs(GLFWwindow *mainWindow);
    ~Inputs();
    void Update(float dt);

    // directions
    glm::vec3 direction, right, up;

    // position
    glm::vec3 position = glm::vec3(0, 0, 5);

    bool showUI = false;

    // field of view
    float FoV;
};

Inputs::Inputs(GLFWwindow *mainWindow)
{
    win = mainWindow;
    glfwGetWindowSize(win, &winWidth, &winHeight);
    glfwSetCursorPos(win, winWidth / 2, winHeight / 2);
}

Inputs::~Inputs()
{
}

void Inputs::Update(float dt)
{
    if(!showUI)
    {
        // Get mouse position
        glfwGetCursorPos(win, &xpos, &ypos);
        // Reset mouse position
        glfwSetCursorPos(win, winWidth / 2, winHeight / 2);
    }
    
    horizontalAngle += mouseSpeed * dt * float(winWidth / 2 - xpos);
    verticalAngle += mouseSpeed * dt * float(winHeight / 2 - ypos);

    // Direction : Spherical coordinates to Cartesian coordinates conversion
    direction = glm::vec3(
        cos(verticalAngle) * sin(horizontalAngle),
        sin(verticalAngle),
        cos(verticalAngle) * cos(horizontalAngle));

    // Right vector
    right = glm::vec3(
        sin(horizontalAngle - 3.14f / 2.0f),
        0,
        cos(horizontalAngle - 3.14f / 2.0f));

    // Up vector : perpendicular to both direction and right
    up = glm::cross(right, direction);

    {
        int tabState = glfwGetKey(win, GLFW_KEY_TAB);

        // Move forward
        if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS)
        {
            position += direction * dt * speed;
        }
        // Move backward
        if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS)
        {
            position -= direction * dt * speed;
        }
        // Strafe right
        if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS)
        {
            position += right * dt * speed;
        }
        // Strafe left
        if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS)
        {
            position -= right * dt * speed;
        }
        if(tabState == GLFW_PRESS && !tabWasDown)
        {
            showUI = !showUI;
            glfwSetInputMode(win, GLFW_CURSOR, (showUI ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN));
            glfwSetCursorPos(win, winWidth / 2, winHeight / 2);
        }
        if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(win, GLFW_TRUE);
        }
        
        tabWasDown = (tabState == GLFW_PRESS);
    }
}
