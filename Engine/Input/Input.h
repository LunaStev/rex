#pragma once
#include <SDL2/SDL.h>
#include "InputKeys.h"
#include <unordered_map>
#include <array>

class Input {
public:
    Input();
    void update(bool& isRunning);

    bool isKeyHeld(RexKey key) const;
    bool isKeyPressed(RexKey key) const;
    bool isKeyReleased(RexKey key) const;

private:
    const Uint8* keyboard = nullptr;
    std::array<Uint8, SDL_NUM_SCANCODES> prevKeyboard{};
    std::unordered_map<RexKey, SDL_Scancode> keyMap;

    void setupKeyMap();
};
