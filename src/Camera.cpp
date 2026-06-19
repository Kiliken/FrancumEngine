#include "Camera.h"


Camera::Camera(SDL_Window *mainWindow)
{
    win = mainWindow;

    SDL_GetWindowSize(win, &winWidth, &winHeight);
    SDL_WarpMouseInWindow(win, winWidth / 2.0f, winHeight / 2.0f);
}

Camera::~Camera()
{
    glDeleteBuffers(1, &UBOID);
}

void Camera::Update(float dt)
{
    const bool *key_states = SDL_GetKeyboardState(NULL);

    if(!showUI)
    {
        SDL_HideCursor();
        SDL_SetWindowRelativeMouseMode(win, true);

        Uint32 mouseInputs = SDL_GetMouseState(&xpos, &ypos);
        SDL_WarpMouseInWindow(win, winWidth / 2.0f, winHeight / 2.0f);

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
        bool tabIsDown = key_states[SDL_SCANCODE_TAB];
        // Move forward
        if (key_states[SDL_SCANCODE_W])
        {
            position += direction * dt * speed;
        }
        // Move backward
        if (key_states[SDL_SCANCODE_S])
        {
            position -= direction * dt * speed;
        }
        if (key_states[SDL_SCANCODE_E])
        {
            position += up * dt * speed;
        }
        // Move Down left
        if (key_states[SDL_SCANCODE_Q])
        {
            position -= up * dt * speed;
        }
        // Strafe right
        if (key_states[SDL_SCANCODE_D])
        {
            position += right * dt * speed;
        }
        // Strafe left
        if (key_states[SDL_SCANCODE_A])
        {
            position -= right * dt * speed;
        }
        if (tabIsDown && !tabWasDown)
        {
            showUI = !showUI;
            SDL_ShowCursor();
            SDL_SetWindowRelativeMouseMode(win, false);
            SDL_WarpMouseInWindow(win, winWidth / 2.0f, winHeight / 2.0f);
        }
        if (key_states[SDL_SCANCODE_ESCAPE])
        {
            SDL_Event e; 
            e.type = SDL_EVENT_QUIT;
            SDL_PushEvent(&e);
        }

        tabWasDown = tabIsDown;
    }

    view = glm::lookAt(
        position,             // Camera is here
        position + direction, // and looks here : at the same position, plus "direction"
        up                    // Head is up (set to 0,-1,0 to look upside-down)
    );

    UBOdata.P = projection;
    UBOdata.V = view;
}

void Camera::BindToShader()
{
    glGenBuffers(1, &UBOID);
    glBindBuffer(GL_UNIFORM_BUFFER, UBOID);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraUBO), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, UBOID);
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