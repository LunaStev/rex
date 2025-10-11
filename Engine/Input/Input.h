#pragma once
#include <SDL2/SDL.h>
#include "InputKeys.h"
#include <unordered_map>

class Input {
public:
    Input();

    void update(bool& isRunning);
    bool isKeyHeld(RexKey key);
    bool isKeyPressed(RexKey key);
    bool isKeyReleased(RexKey key);

private:
    const Uint8* state = nullptr;
    std::unordered_map<RexKey, SDL_Scancode> keyMap;
    std::unordered_map<RexKey, bool> prevState;

    void setupKeyMap();
};
