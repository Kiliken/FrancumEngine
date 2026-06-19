#include "Inputs.h"

Inputs::Inputs(SDL_Window *mainWindow)
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
    SDL_Keycode sdlKey = static_cast<SDL_Keycode>(key);
    SDL_Scancode scancode = SDL_GetScancodeFromKey(sdlKey, nullptr);
    
    int numkeys;
    const bool* state = SDL_GetKeyboardState(&numkeys);
    
    if (state && scancode > SDL_SCANCODE_UNKNOWN && scancode < numkeys)
    {
        return state[scancode];
    }
    
    return false;
}