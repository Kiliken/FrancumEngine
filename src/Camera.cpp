#include "Camera.h"


Camera::Camera(GLFWwindow *mainWindow)
{
    win = mainWindow;
    glfwGetWindowSize(win, &winWidth, &winHeight);
    glfwSetCursorPos(win, winWidth / 2, winHeight / 2);
}

Camera::~Camera()
{
}

void Camera::Update(float dt)
{
    if(!showUI)
    {
        // Get mouse position
        glfwGetCursorPos(win, &xpos, &ypos);
        // Reset mouse position
        glfwSetCursorPos(win, winWidth / 2, winHeight / 2);

        horizontalAngle += mouseSpeed * dt * float(winWidth / 2 - xpos);
        verticalAngle += mouseSpeed * dt * float(winHeight / 2 - ypos);
    }

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
            glfwSetInputMode(win, GLFW_CURSOR, (showUI ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED));
            glfwSetCursorPos(win, winWidth / 2, winHeight / 2);
        }
        if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(win, GLFW_TRUE);
        }
        
        tabWasDown = (tabState == GLFW_PRESS);
    }
}

void Camera::resizeView(int w, int h) {
    winWidth = w;
    winHeight = h;
}

void Camera::SetPosition(const float &x, const float &y, const float &z)
{
    position = glm::vec3(x, y, z);
}

void Camera::SetRotation(const float &x, const float &y)
{
    verticalAngle = y;
    horizontalAngle = x;
}

void Camera::SetProjMode(const char *mode){
    if(std::strcmp(mode, "PERSP") == 0)
        projection = glm::perspective(glm::radians(60.0f), 16.0f / 9.0f, 0.1f, 100.0f);
    else if(std::strcmp(mode, "ORTHO") == 0)
        projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 100.0f);
}