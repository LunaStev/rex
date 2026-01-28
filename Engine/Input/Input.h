#pragma once
#include <SDL2/SDL.h>
#include "InputKeys.h"

#include <unordered_map>
#include <array>
#include <string>
#include <cstdint>

struct RexKeyHash {
    std::size_t operator()(RexKey k) const noexcept {
        return static_cast<std::size_t>(k);
    }
};

class Input {
public:
    Input();

    // 엔진 루프에서 매 프레임 호출
    void update(bool& isRunning);

    // ---- Keyboard ----
    bool isKeyHeld(RexKey key) const;
    bool isKeyPressed(RexKey key) const;
    bool isKeyReleased(RexKey key) const;

    // 엔진 기본 종료 키(ESC) 옵션
    void setCloseOnEscape(bool v) { closeOnEscape = v; }
    bool getCloseOnEscape() const { return closeOnEscape; }

    // ---- Window ----
    bool wasWindowResized() const { return windowResized; }
    int  getWindowW() const { return windowW; }
    int  getWindowH() const { return windowH; }

    // ---- Mouse (필수급이라 같이 넣음) ----
    int getMouseX() const { return mouseX; }
    int getMouseY() const { return mouseY; }
    int getMouseDX() const { return mouseDX; }
    int getMouseDY() const { return mouseDY; }
    int getWheelX() const { return wheelX; }
    int getWheelY() const { return wheelY; }

    bool isMouseHeld(uint8_t button) const;     // SDL_BUTTON_LEFT 등
    bool isMousePressed(uint8_t button) const;
    bool isMouseReleased(uint8_t button) const;

    // ---- Text Input (UI/콘솔/채팅에 필요) ----
    void startTextInput();
    void stopTextInput();
    bool isTextInputActive() const { return textInputActive; }
    const std::string& getTextInput() const { return textInput; } // 이번 프레임에 들어온 텍스트 누적

private:
    void setupKeyMap();
    SDL_Scancode toScan(RexKey key) const;

private:
    // 키 상태를 “스냅샷 배열”로 관리 (stuck 방지)
    std::array<Uint8, SDL_NUM_SCANCODES> curKeyboard{};
    std::array<Uint8, SDL_NUM_SCANCODES> prevKeyboard{};

    std::unordered_map<RexKey, SDL_Scancode, RexKeyHash> keyMap;

    bool closeOnEscape = true;

    // 윈도우/포커스
    bool windowResized = false;
    int windowW = 0;
    int windowH = 0;

    // 마우스
    int mouseX = 0, mouseY = 0;
    int mouseDX = 0, mouseDY = 0;
    int wheelX = 0, wheelY = 0;

    // 버튼 상태: SDL 버튼은 1~5가 흔함. 넉넉하게 8칸
    std::array<uint8_t, 8> curMouse{};
    std::array<uint8_t, 8> prevMouse{};

    // 텍스트 입력
    bool textInputActive = false;
    std::string textInput;

private:
    void clearAllStates(); // 포커스 로스트에서 강제 초기화
};
