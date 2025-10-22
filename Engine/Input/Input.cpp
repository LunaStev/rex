#include "Input.h"

Input::Input() {
    setupKeyMap();
}

void Input::setupKeyMap() {
    // Add mappings for A-Z
    for (int i = 0; i < 26; ++i) {
        keyMap[static_cast<RexKey>(static_cast<int>(RexKey::A) + i)] = static_cast<SDL_Scancode>(SDL_SCANCODE_A + i);
    }

    // Add mappings for 0-9
    for (int i = 0; i < 10; ++i) {
        keyMap[static_cast<RexKey>(static_cast<int>(RexKey::N1) + i)] = static_cast<SDL_Scancode>(SDL_SCANCODE_1 + i);
    }

    // Add mappings for keypad 0-9
    for (int i = 0; i < 10; ++i) {
        keyMap[static_cast<RexKey>(static_cast<int>(RexKey::KP_1) + i)] = static_cast<SDL_Scancode>(SDL_SCANCODE_KP_1 + i);
    }

    // Add mappings for F1-F24
    for (int i = 0; i < 24; ++i) {
        keyMap[static_cast<RexKey>(static_cast<int>(RexKey::F1) + i)] = static_cast<SDL_Scancode>(SDL_SCANCODE_F1 + i);
    }

    // Add mappings for INTERNATIONAL1-INTERNATIONAL9
    for (int i = 0; i < 9; ++i) {
        keyMap[static_cast<RexKey>(static_cast<int>(RexKey::INTERNATIONAL1) + i)] = static_cast<SDL_Scancode>(SDL_SCANCODE_INTERNATIONAL1 + i);
    }

    // Add mappings for LANG1-LANG9
    for (int i = 0; i < 9; ++i) {
        keyMap[static_cast<RexKey>(static_cast<int>(RexKey::LANG1) + i)] = static_cast<SDL_Scancode>(SDL_SCANCODE_LANG1 + i);
    }

    
    for (int i = 0; i < 6; ++i) {
        keyMap[static_cast<RexKey>(static_cast<int>(RexKey::KP_A) + i)] = static_cast<SDL_Scancode>(SDL_SCANCODE_KP_A + i);
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

        {RexKey::APPLICATION, SDL_SCANCODE_APPLICATION},
        {RexKey::POWER, SDL_SCANCODE_POWER},

        {RexKey::KP_EQUALS, SDL_SCANCODE_KP_EQUALS},
        {RexKey::EXECUTE, SDL_SCANCODE_EXECUTE},
        {RexKey::HELP, SDL_SCANCODE_HELP},
        {RexKey::MENU, SDL_SCANCODE_MENU},
        {RexKey::SELECT, SDL_SCANCODE_SELECT},
        {RexKey::STOP, SDL_SCANCODE_STOP},
        {RexKey::AGAIN, SDL_SCANCODE_AGAIN},
        {RexKey::UNDO, SDL_SCANCODE_UNDO},
        {RexKey::CUT, SDL_SCANCODE_CUT},
        {RexKey::COPY, SDL_SCANCODE_COPY},
        {RexKey::PASTE, SDL_SCANCODE_PASTE},
        {RexKey::FIND, SDL_SCANCODE_FIND},
        {RexKey::MUTE, SDL_SCANCODE_MUTE},
        {RexKey::MUTE, SDL_SCANCODE_MUTE},
        {RexKey::VOLUMEUP, SDL_SCANCODE_VOLUMEUP},
        {RexKey::VOLUMEDOWN, SDL_SCANCODE_VOLUMEDOWN},
        
        {RexKey::KP_COMMA, SDL_SCANCODE_KP_COMMA},
        {RexKey::KP_EQUALSAS400, SDL_SCANCODE_KP_EQUALSAS400},
        
        {RexKey::ALTERASE, SDL_SCANCODE_ALTERASE},
        {RexKey::SYSREQ, SDL_SCANCODE_SYSREQ},
        {RexKey::CANCEL, SDL_SCANCODE_CANCEL},
        {RexKey::CLEAR, SDL_SCANCODE_CLEAR},
        {RexKey::PRIOR, SDL_SCANCODE_PRIOR},
        {RexKey::RETURN2, SDL_SCANCODE_RETURN2},
        {RexKey::SEPARATOR, SDL_SCANCODE_SEPARATOR},
        {RexKey::OUT, SDL_SCANCODE_OUT},
        {RexKey::OPER, SDL_SCANCODE_OPER},
        {RexKey::CLEARAGAIN, SDL_SCANCODE_CLEARAGAIN},
        {RexKey::CRSEL, SDL_SCANCODE_CRSEL},
        {RexKey::EXSEL, SDL_SCANCODE_EXSEL},
        
        {RexKey::KP_00, SDL_SCANCODE_KP_00},
        {RexKey::KP_000, SDL_SCANCODE_KP_000},
        {RexKey::THOUSANDSSEPARATOR, SDL_SCANCODE_THOUSANDSSEPARATOR},
        {RexKey::DECIMALSEPARATOR, SDL_SCANCODE_DECIMALSEPARATOR},
        {RexKey::CURRENCYUNIT, SDL_SCANCODE_CURRENCYUNIT},
        {RexKey::CURRENCYSUBUNIT, SDL_SCANCODE_CURRENCYSUBUNIT},
        {RexKey::KP_LEFTPAREN, SDL_SCANCODE_KP_LEFTPAREN},
        {RexKey::KP_RIGHTPAREN, SDL_SCANCODE_KP_RIGHTPAREN},
        {RexKey::KP_LEFTBRACE, SDL_SCANCODE_KP_LEFTBRACE},
        {RexKey::KP_RIGHTBRACE, SDL_SCANCODE_KP_RIGHTBRACE},
        {RexKey::KP_TAB, SDL_SCANCODE_KP_TAB},
        {RexKey::KP_BACKSPACE, SDL_SCANCODE_KP_BACKSPACE},
        {RexKey::KP_XOR, SDL_SCANCODE_KP_XOR},
        {RexKey::KP_POWER, SDL_SCANCODE_KP_POWER},
        {RexKey::KP_PERCENT, SDL_SCANCODE_KP_PERCENT},
        {RexKey::KP_LESS, SDL_SCANCODE_KP_LESS},
        {RexKey::KP_GREATER, SDL_SCANCODE_KP_GREATER},
        {RexKey::KP_AMPERSAND, SDL_SCANCODE_KP_AMPERSAND},
        {RexKey::KP_DBLAMPERSAND, SDL_SCANCODE_KP_DBLAMPERSAND},
        {RexKey::KP_VERTICALBAR, SDL_SCANCODE_KP_VERTICALBAR},
        {RexKey::KP_DBLVERTICALBAR, SDL_SCANCODE_KP_DBLVERTICALBAR},
        {RexKey::KP_COLON, SDL_SCANCODE_KP_COLON},
        {RexKey::KP_HASH, SDL_SCANCODE_KP_HASH},
        {RexKey::KP_SPACE, SDL_SCANCODE_KP_SPACE},
        {RexKey::KP_AT, SDL_SCANCODE_KP_AT},
        {RexKey::KP_EXCALM, SDL_SCANCODE_KP_EXCLAM},
        {RexKey::KP_MEMSTORE, SDL_SCANCODE_KP_MEMSTORE},
        {RexKey::KP_MEMRECALL, SDL_SCANCODE_KP_MEMRECALL},
        {RexKey::KP_MEMCLEAR, SDL_SCANCODE_KP_MEMCLEAR},
        {RexKey::KP_MEMADD, SDL_SCANCODE_KP_MEMADD},
        {RexKey::KP_MEMSUBTRACT, SDL_SCANCODE_KP_MEMSUBTRACT},
        {RexKey::KP_MEMMULTIPLY, SDL_SCANCODE_KP_MEMMULTIPLY},
        {RexKey::KP_MEMDIVIDE, SDL_SCANCODE_KP_MEMDIVIDE},
        {RexKey::KP_PLUSMINUS, SDL_SCANCODE_KP_PLUSMINUS},
        {RexKey::KP_CLEAR, SDL_SCANCODE_KP_CLEAR},
        {RexKey::KP_CLEARENTRY, SDL_SCANCODE_KP_CLEARENTRY},
        {RexKey::KP_BINARY, SDL_SCANCODE_KP_BINARY},
        {RexKey::KP_OCTAL, SDL_SCANCODE_KP_OCTAL},
        {RexKey::KP_DECIMAL, SDL_SCANCODE_KP_DECIMAL},
        {RexKey::KP_HEXADECIMAL, SDL_SCANCODE_KP_HEXADECIMAL},

        {RexKey::LSHIFT, SDL_SCANCODE_LSHIFT},
        {RexKey::LCTRL, SDL_SCANCODE_LCTRL},
        {RexKey::LALT, SDL_SCANCODE_LALT},
        {RexKey::LGUI, SDL_SCANCODE_LGUI},
        {RexKey::RCTRL, SDL_SCANCODE_RCTRL},
        {RexKey::RSHIFT, SDL_SCANCODE_RSHIFT},
        {RexKey::RALT, SDL_SCANCODE_RALT},
        {RexKey::RGUI, SDL_SCANCODE_RGUI},

        {RexKey::MODE, SDL_SCANCODE_MODE},

        {RexKey::AUDIONEXT, SDL_SCANCODE_AUDIONEXT},
        {RexKey::AUDIOPREV, SDL_SCANCODE_AUDIOPREV},
        {RexKey::AUDIOSTOP, SDL_SCANCODE_AUDIOSTOP},
        {RexKey::AUDIOPLAY, SDL_SCANCODE_AUDIOPLAY},
        {RexKey::AUDIOMUTE, SDL_SCANCODE_AUDIOMUTE},
        {RexKey::WWW, SDL_SCANCODE_WWW},
        {RexKey::MAIL, SDL_SCANCODE_MAIL},
        {RexKey::CALCULATOR, SDL_SCANCODE_CALCULATOR},
        {RexKey::COMPUTER, SDL_SCANCODE_COMPUTER},
        {RexKey::AC_SEARCH, SDL_SCANCODE_AC_SEARCH},
        {RexKey::AC_HOME, SDL_SCANCODE_AC_HOME},
        {RexKey::AC_BACK, SDL_SCANCODE_AC_BACK},
        {RexKey::AC_FORWARD, SDL_SCANCODE_AC_FORWARD},
        {RexKey::AC_STOP, SDL_SCANCODE_AC_STOP},
        {RexKey::AC_REFRESH, SDL_SCANCODE_AC_REFRESH},
        {RexKey::AC_BOOKMARKS, SDL_SCANCODE_AC_BOOKMARKS},

        {RexKey::BRIGHTNESSDOWN, SDL_SCANCODE_BRIGHTNESSDOWN},
        {RexKey::BRIGHTNESSUP, SDL_SCANCODE_BRIGHTNESSUP},
        {RexKey::DISPLAYSWITCH, SDL_SCANCODE_DISPLAYSWITCH},

        {RexKey::KBDILLUMTOGGLE, SDL_SCANCODE_KBDILLUMTOGGLE},
        {RexKey::KBDILLUMDOWN, SDL_SCANCODE_KBDILLUMDOWN},
        {RexKey::KBDILLUMUP, SDL_SCANCODE_KBDILLUMUP},
        {RexKey::EJECT, SDL_SCANCODE_EJECT},
        {RexKey::SLEEP, SDL_SCANCODE_SLEEP},

        {RexKey::APP1, SDL_SCANCODE_APP1},
        {RexKey::APP2, SDL_SCANCODE_APP2},

        {RexKey::AUDIOREWIND, SDL_SCANCODE_AUDIOREWIND},
        {RexKey::AUDIOFASTFORWARD, SDL_SCANCODE_AUDIOFASTFORWARD},

        {RexKey::SOFTLEFT, SDL_SCANCODE_SOFTLEFT},

        {RexKey::SOFTRIGHT, SDL_SCANCODE_SOFTRIGHT},

        {RexKey::CALL, SDL_SCANCODE_CALL},
        {RexKey::ENDCALL, SDL_SCANCODE_ENDCALL},

        {RexKey::SCANCODES, SDL_NUM_SCANCODES},
    };

    for (auto& [rex, sdl] : extraKeys) {
        keyMap[rex] = sdl;
    }
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
