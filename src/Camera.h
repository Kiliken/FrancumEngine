#pragma once

#include <cstring>
#include <glad/gl.h>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct CameraUBO
{
    glm::mat4 V;
    glm::mat4 P;
};

class Camera
{
private:
    // window reference
    SDL_Window *win;

    // win size
    int winWidth, winHeight;

    // mouse Pos
    float xpos, ypos;
    // view
    glm::mat4 view = glm::lookAt(
        glm::vec3(4, 3, 3), // Camera is at (4,3,3), in World Space
        glm::vec3(0, 0, 0), // and looks at the origin
        glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

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
    Camera(SDL_Window *mainWindow);
    ~Camera();

    // directions
    glm::vec3 direction, right, up;

    // position
    glm::vec3 position = glm::vec3(0, 0, 5);

    glm::mat4 projection = glm::perspective(glm::radians(60.0f), 16.0f / 9.0f, 0.1f, 100.0f);

    bool showUI = false;

    // field of view
    float FoV;
    bool isPerspective = true;

    GLuint UBOID;
    CameraUBO UBOdata;


    void Update(float dt);
    void BindToShader();
    void resizeView(int w, int h);
    void SetPosition(const float &x, const float &y, const float &z);
    void SetRotation(const float &x, const float &y);
    void SetProjMode(const char *mode);
};