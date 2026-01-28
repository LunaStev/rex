#include "Input.h"
#include <cstring> // memcpy

Input::Input() {
    setupKeyMap();
    curKeyboard.fill(0);
    prevKeyboard.fill(0);
    curMouse.fill(0);
    prevMouse.fill(0);
}

void Input::setupKeyMap() {
    keyMap.clear();
    keyMap.reserve(256);

    // A-Z
    for (int i = 0; i < 26; ++i) {
        keyMap[static_cast<RexKey>(static_cast<int>(RexKey::A) + i)] =
            static_cast<SDL_Scancode>(SDL_SCANCODE_A + i);
    }

    // 1-0
    for (int i = 0; i < 10; ++i) {
        keyMap[static_cast<RexKey>(static_cast<int>(RexKey::N1) + i)] =
            static_cast<SDL_Scancode>(SDL_SCANCODE_1 + i);
    }

    // Keypad 1-0
    for (int i = 0; i < 10; ++i) {
        keyMap[static_cast<RexKey>(static_cast<int>(RexKey::KP_1) + i)] =
            static_cast<SDL_Scancode>(SDL_SCANCODE_KP_1 + i);
    }

    // F1-F24
    for (int i = 0; i < 24; ++i) {
        keyMap[static_cast<RexKey>(static_cast<int>(RexKey::F1) + i)] =
            static_cast<SDL_Scancode>(SDL_SCANCODE_F1 + i);
    }

    // INTERNATIONAL1-9
    for (int i = 0; i < 9; ++i) {
        keyMap[static_cast<RexKey>(static_cast<int>(RexKey::INTERNATIONAL1) + i)] =
            static_cast<SDL_Scancode>(SDL_SCANCODE_INTERNATIONAL1 + i);
    }

    // LANG1-9
    for (int i = 0; i < 9; ++i) {
        keyMap[static_cast<RexKey>(static_cast<int>(RexKey::LANG1) + i)] =
            static_cast<SDL_Scancode>(SDL_SCANCODE_LANG1 + i);
    }

    // KP_A - KP_F (hex keypad)
    for (int i = 0; i < 6; ++i) {
        keyMap[static_cast<RexKey>(static_cast<int>(RexKey::KP_A) + i)] =
            static_cast<SDL_Scancode>(SDL_SCANCODE_KP_A + i);
    }

    const std::pair<RexKey, SDL_Scancode> extraKeys[] = {
        {RexKey::RETURN, SDL_SCANCODE_RETURN},
        {RexKey::ESCAPE, SDL_SCANCODE_ESCAPE},
        {RexKey::BACKSPACE, SDL_SCANCODE_BACKSPACE},
        {RexKey::TAB, SDL_SCANCODE_TAB},
        {RexKey::SPACE, SDL_SCANCODE_SPACE},

        {RexKey::MINUS, SDL_SCANCODE_MINUS},
        {RexKey::EQUALS, SDL_SCANCODE_EQUALS},
        {RexKey::LBRACKET, SDL_SCANCODE_LEFTBRACKET},
        {RexKey::RBRACKET, SDL_SCANCODE_RIGHTBRACKET},
        {RexKey::BACKSLASH, SDL_SCANCODE_BACKSLASH},
        {RexKey::NONUSHASH, SDL_SCANCODE_NONUSHASH},

        {RexKey::SEMICOLON, SDL_SCANCODE_SEMICOLON},
        {RexKey::APOSTROPHE, SDL_SCANCODE_APOSTROPHE},
        {RexKey::GRAVE, SDL_SCANCODE_GRAVE},
        {RexKey::COMMA, SDL_SCANCODE_COMMA},
        {RexKey::PERIOD, SDL_SCANCODE_PERIOD},
        {RexKey::SLASH, SDL_SCANCODE_SLASH},

        {RexKey::CAPSLOCK, SDL_SCANCODE_CAPSLOCK},

        {RexKey::PRINTSCREEN, SDL_SCANCODE_PRINTSCREEN},
        {RexKey::SCROLLLOCK, SDL_SCANCODE_SCROLLLOCK},
        {RexKey::PAUSE, SDL_SCANCODE_PAUSE},
        {RexKey::INSERT, SDL_SCANCODE_INSERT},

        {RexKey::HOME, SDL_SCANCODE_HOME},
        {RexKey::PAGEUP, SDL_SCANCODE_PAGEUP},
        {RexKey::DELETE, SDL_SCANCODE_DELETE},
        {RexKey::END, SDL_SCANCODE_END},
        {RexKey::PAGEDOWN, SDL_SCANCODE_PAGEDOWN},
        {RexKey::RIGHT, SDL_SCANCODE_RIGHT},
        {RexKey::LEFT, SDL_SCANCODE_LEFT},
        {RexKey::DOWN, SDL_SCANCODE_DOWN},
        {RexKey::UP, SDL_SCANCODE_UP},

        {RexKey::NUMLOCKCLEAR, SDL_SCANCODE_NUMLOCKCLEAR},

        {RexKey::KP_DIVIDE, SDL_SCANCODE_KP_DIVIDE},
        {RexKey::KP_MULTIPLY, SDL_SCANCODE_KP_MULTIPLY},
        {RexKey::KP_MINUS, SDL_SCANCODE_KP_MINUS},
        {RexKey::KP_PLUS, SDL_SCANCODE_KP_PLUS},
        {RexKey::KP_ENTER, SDL_SCANCODE_KP_ENTER},
        {RexKey::KP_PERIOD, SDL_SCANCODE_KP_PERIOD},

        {RexKey::NONUSBACKSLASH, SDL_SCANCODE_NONUSBACKSLASH},

        {RexKey::LSHIFT, SDL_SCANCODE_LSHIFT},
        {RexKey::LCTRL, SDL_SCANCODE_LCTRL},
        {RexKey::LALT, SDL_SCANCODE_LALT},
        {RexKey::LGUI, SDL_SCANCODE_LGUI},
        {RexKey::RCTRL, SDL_SCANCODE_RCTRL},
        {RexKey::RSHIFT, SDL_SCANCODE_RSHIFT},
        {RexKey::RALT, SDL_SCANCODE_RALT},
        {RexKey::RGUI, SDL_SCANCODE_RGUI},
    };

    for (auto& p : extraKeys) keyMap[p.first] = p.second;
}

SDL_Scancode Input::toScan(RexKey key) const {
    auto it = keyMap.find(key);
    if (it == keyMap.end()) return SDL_SCANCODE_UNKNOWN;
    return it->second;
}

void Input::clearAllStates() {
    curKeyboard.fill(0);
    prevKeyboard.fill(0);
    curMouse.fill(0);
    prevMouse.fill(0);
    mouseDX = mouseDY = 0;
    wheelX = wheelY = 0;
    textInput.clear();
}

void Input::update(bool& isRunning) {
    // prev <- cur (스냅샷)
    prevKeyboard = curKeyboard;
    prevMouse = curMouse;

    // per-frame reset
    windowResized = false;
    mouseDX = mouseDY = 0;
    wheelX = wheelY = 0;
    textInput.clear();

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
        case SDL_QUIT:
            isRunning = false;
            break;

        case SDL_KEYDOWN:
            if (closeOnEscape && !e.key.repeat && e.key.keysym.sym == SDLK_ESCAPE) {
                isRunning = false;
            }
            break;

        case SDL_WINDOWEVENT:
            if (e.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
                // ALT+TAB 등에서 키 stuck 방지
                clearAllStates();
            } else if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED ||
                       e.window.event == SDL_WINDOWEVENT_RESIZED) {
                windowResized = true;
                windowW = e.window.data1;
                windowH = e.window.data2;
            } else if (e.window.event == SDL_WINDOWEVENT_CLOSE) {
                isRunning = false;
            }
            break;

        case SDL_MOUSEMOTION:
            mouseX = e.motion.x;
            mouseY = e.motion.y;
            mouseDX += e.motion.xrel;
            mouseDY += e.motion.yrel;
            break;

        case SDL_MOUSEBUTTONDOWN: {
            uint8_t b = e.button.button;
            if (b < curMouse.size()) curMouse[b] = 1;
            break;
        }
        case SDL_MOUSEBUTTONUP: {
            uint8_t b = e.button.button;
            if (b < curMouse.size()) curMouse[b] = 0;
            break;
        }
        case SDL_MOUSEWHEEL:
            wheelX += e.wheel.x;
            wheelY += e.wheel.y;
            break;

        case SDL_TEXTINPUT:
            if (textInputActive) textInput += e.text.text;
            break;

        default:
            break;
        }
    }

    // 키보드 상태는 항상 마지막에 스냅샷 뜸
    SDL_PumpEvents();
    const Uint8* sdlState = SDL_GetKeyboardState(nullptr);
    if (sdlState) {
        std::memcpy(curKeyboard.data(), sdlState, SDL_NUM_SCANCODES);
    } else {
        curKeyboard.fill(0);
    }
}

bool Input::isKeyHeld(RexKey key) const {
    SDL_Scancode sc = toScan(key);
    if (sc == SDL_SCANCODE_UNKNOWN) return false;
    return curKeyboard[sc] != 0;
}

bool Input::isKeyPressed(RexKey key) const {
    SDL_Scancode sc = toScan(key);
    if (sc == SDL_SCANCODE_UNKNOWN) return false;
    return (curKeyboard[sc] != 0) && (prevKeyboard[sc] == 0);
}

bool Input::isKeyReleased(RexKey key) const {
    SDL_Scancode sc = toScan(key);
    if (sc == SDL_SCANCODE_UNKNOWN) return false;
    return (curKeyboard[sc] == 0) && (prevKeyboard[sc] != 0);
}

// ---- Mouse ----
bool Input::isMouseHeld(uint8_t button) const {
    if (button >= curMouse.size()) return false;
    return curMouse[button] != 0;
}

bool Input::isMousePressed(uint8_t button) const {
    if (button >= curMouse.size()) return false;
    return (curMouse[button] != 0) && (prevMouse[button] == 0);
}

bool Input::isMouseReleased(uint8_t button) const {
    if (button >= curMouse.size()) return false;
    return (curMouse[button] == 0) && (prevMouse[button] != 0);
}

// ---- Text Input ----
void Input::startTextInput() {
    if (textInputActive) return;
    textInputActive = true;
    SDL_StartTextInput();
}

void Input::stopTextInput() {
    if (!textInputActive) return;
    textInputActive = false;
    SDL_StopTextInput();
}
