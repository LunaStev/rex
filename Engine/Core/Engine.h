#pragma once
#include <SDL2/SDL.h>
#include <iostream>
#include "../Graphics/Graphics.h"
#include "../Input/Input.h"
#include "Game.h"

class Engine {
public:
    Engine();
    ~Engine();

    bool init(const char* title, int width, int height);

    template<typename T>
    static void run(const char* title = "My 2D Engine", int width = 800, int height = 600) {
        Engine engine;
        if (!engine.init(title, width, height)) {
            return;
        }
        T game; // 사용자가 만든 게임 클래스
        engine.mainLoop(game);
    }

    void quit();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool isRunning;

    Graphics graphics;
    Input input;

    void mainLoop(Game& game);
};