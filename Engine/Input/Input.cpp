#include "Input.h"

Input::Input() {
    setupKeyMap();
}

void Input::setupKeyMap() {
    keyMap[RexKey::A]       = SDL_SCANCODE_A;
    keyMap[RexKey::B]       = SDL_SCANCODE_B;
    keyMap[RexKey::C]       = SDL_SCANCODE_C;
    keyMap[RexKey::D]       = SDL_SCANCODE_D;
    keyMap[RexKey::E]       = SDL_SCANCODE_E;
    keyMap[RexKey::F]       = SDL_SCANCODE_F;
    keyMap[RexKey::G]       = SDL_SCANCODE_G;
    keyMap[RexKey::H]       = SDL_SCANCODE_H;
    keyMap[RexKey::I]       = SDL_SCANCODE_I;
    keyMap[RexKey::J]       = SDL_SCANCODE_J;
    keyMap[RexKey::K]       = SDL_SCANCODE_K;
    keyMap[RexKey::L]       = SDL_SCANCODE_L;
    keyMap[RexKey::M]       = SDL_SCANCODE_M;
    keyMap[RexKey::N]       = SDL_SCANCODE_N;
    keyMap[RexKey::O]       = SDL_SCANCODE_O;
    keyMap[RexKey::P]       = SDL_SCANCODE_P;
    keyMap[RexKey::Q]       = SDL_SCANCODE_Q;
    keyMap[RexKey::R]       = SDL_SCANCODE_R;
    keyMap[RexKey::S]       = SDL_SCANCODE_S;
    keyMap[RexKey::T]       = SDL_SCANCODE_T;
    keyMap[RexKey::U]       = SDL_SCANCODE_U;
    keyMap[RexKey::V]       = SDL_SCANCODE_V;
    keyMap[RexKey::W]       = SDL_SCANCODE_W;
    keyMap[RexKey::X]       = SDL_SCANCODE_X;
    keyMap[RexKey::Y]       = SDL_SCANCODE_Y;
    keyMap[RexKey::Z]       = SDL_SCANCODE_Z;
    keyMap[RexKey::DOWN]    = SDL_SCANCODE_DOWN;
    keyMap[RexKey::LEFT]    = SDL_SCANCODE_LEFT;
    keyMap[RexKey::RIGHT]   = SDL_SCANCODE_RIGHT;
    keyMap[RexKey::SPACE]   = SDL_SCANCODE_SPACE;
    keyMap[RexKey::ESCAPE]  = SDL_SCANCODE_ESCAPE;
    keyMap[RexKey::RETURN]  = SDL_SCANCODE_RETURN;
    keyMap[RexKey::SHIFT]   = SDL_SCANCODE_LSHIFT;
    keyMap[RexKey::CTRL]    = SDL_SCANCODE_LCTRL;
    keyMap[RexKey::ALT]     = SDL_SCANCODE_LALT;
}

void Input::update(bool& isRunning) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT)
            isRunning = false;
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
            isRunning = false;
    }

    const Uint8* current = SDL_GetKeyboardState(NULL);

    for (auto& [rexKey, scancode] : keyMap) {
        bool held = current[scancode];
        prevState[rexKey] = held;
    }

    state = current;
}

bool Input::isKeyHeld(RexKey key) {
    auto it = keyMap.find(key);
    if (it == keyMap.end() || !state) return false;
    return state[it->second];
}

bool Input::isKeyPressed(RexKey key) {
    auto it = keyMap.find(key);
    if (it == keyMap.end() || !state) return false;
    SDL_Scancode code = it->second;
    bool current = state[code];
    bool prev = prevState[key];
    return current && !prev;
}

bool Input::isKeyReleased(RexKey key) {
    auto it = keyMap.find(key);
    if (it == keyMap.end() || !state) return false;
    SDL_Scancode code = it->second;
    bool current = state[code];
    bool prev = prevState[key];
    return !current && prev;
}
