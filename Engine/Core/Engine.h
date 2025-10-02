#pragma once
#include <SDL2/SDL.h>
#include <iostream>

class Engine {
public:
    Engine();
    ~Engine();

    bool init(const char* title, int width, int height);
    void run();
    void quit();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool isRunning;
};