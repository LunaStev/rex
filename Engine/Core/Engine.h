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
#include "Scene.h" // Scene 타입 쓰니까 안전하게 포함

class Engine {
public:
    Engine();
    ~Engine();

    bool init(const char* title, int width, int height);

    template<typename T, typename... Args>
    static void run(const char* title = "My 2D Engine", int width = 800, int height = 600, Args&&... args) {
        Engine engine;
        if (!engine.init(title, width, height)) return;

        if constexpr (std::is_base_of_v<Scene, T>) {
            engine.scenes.replace<T>(std::forward<Args>(args)...);
        } else if constexpr (std::is_base_of_v<Game, T>) {
            engine.scenes.replace(std::make_unique<GameSceneAdapter<T>>(std::forward<Args>(args)...));
        } else {
            static_assert(sizeof(T) == 0, "T must derive from Scene or Game");
        }

        engine.mainLoop();
    }

    void quit();
    void stop() { isRunning = false; }

    Graphics& getGraphics() { return graphics; }
    Input& getInput() { return input; }
    SDL_Renderer* getSDLRenderer() { return renderer; }

    Audio& getAudio() { return audio; }

    // ✅ 이거만 쓰자 (예제도 전부 engine.getAssets()로)
    ResourceManager& getAssets() { return assets; }
    const ResourceManager& getAssets() const { return assets; }

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
