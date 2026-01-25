#pragma once
#include <SDL2/SDL.h>
#include <iostream>
#include "../Graphics/Graphics.h"
#include "../Input/Input.h"
#include "SceneManager.h"
#include <type_traits>
#include "GameSceneAdapter.h"


class Engine {
public:
    Engine();
    ~Engine();

    bool init(const char* title, int width, int height);

    template<typename T>
    static void run(const char* title = "My 2D Engine", int width = 800, int height = 600) {
        Engine engine;
        if (!engine.init(title, width, height)) return;

        if constexpr (std::is_base_of_v<Scene, T>) {
            engine.scenes.replace<T>();
        } else if constexpr (std::is_base_of_v<Game, T>) {
            engine.scenes.replace(std::make_unique<GameSceneAdapter<T>>());
        } else {
            static_assert(sizeof(T) == 0, "T must derive from Scene or Game");
        }
        engine.mainLoop();
    }

    void quit();

    // Controls
    void stop() { isRunning = false; }

    Graphics& getGraphics() { return graphics; }
    Input& getInput() { return input; }
    SDL_Renderer* getSDLRenderer() { return renderer; }

    SceneManager scenes;

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool isRunning;

    Graphics graphics;
    Input input;

    void mainLoop();
};
