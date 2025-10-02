#include "Input.h"

void Input::update(bool& isRunning) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            isRunning = false;
        }
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
            isRunning = false;
        }
    }
    state = SDL_GetKeyboardState(NULL);
}

bool Input::isKeyHeld(SDL_Scancode key) {
    return state && state[key];
}