#pragma once
#include <SDL2/SDL.h>

class Input {
public:
    void update(bool& isRunning);
    bool isKeyHeld(SDL_Scancode key);

private:
    const Uint8* state = nullptr;
};