#pragma once
#include <SDL2/SDL.h>
#include <iostream>
#include <type_traits>

#include "../Graphics/Graphics.h"
#include "../Input/Input.h"

#include "../Audio/Audio.h"
#include "ResourceManager.h"

#include "SceneManager.h"
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
        engine.quit();
    }

    void quit();
    void stop() { isRunning = false; }

    Graphics& getGraphics() { return graphics; }
    Input& getInput() { return input; }
    SDL_Renderer* getSDLRenderer() { return renderer; }

    Audio& getAudio() { return audio; }
    ResourceManager& getAssets() { return assets; }

    SceneManager scenes;

private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    bool isRunning = false;
    bool sdlInited = false;

    int windowW = 0;
    int windowH = 0;

    Graphics graphics;
    Input input;

    Audio audio;
    ResourceManager assets;

    void mainLoop();
    void syncViewportIfNeeded();

private:
    static constexpr double FIXED_DT = 1.0 / 60.0;
};
