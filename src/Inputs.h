#pragma once

#include <glad/gl.h>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Inputs
{
private:
    // window reference
    SDL_Window *win;



public:
    Inputs(SDL_Window *mainWindow);
    ~Inputs();
    void Update(float dt);

    bool IsKeyDown(int key);

};