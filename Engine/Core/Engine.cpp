#include "Engine.h"

Engine::Engine() : window(nullptr), renderer(nullptr), isRunning(false) {}
Engine::~Engine() { quit(); }

bool Engine::init(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL Init Error: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              width, height, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window Error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "Renderer Error: " << SDL_GetError() << std::endl;
        return false;
    }

    graphics.setRenderer(renderer);
    isRunning = true;
    return true;
}

void Engine::mainLoop() {
    Uint32 lastTick = SDL_GetTicks();

    while (isRunning) {
        input.update(isRunning);

        Uint32 currentTick = SDL_GetTicks();
        float dt = (currentTick - lastTick) / 1000.0f;
        lastTick = currentTick;

        // Apply any scene change requests safely
        scenes.applyPending(*this);

        // If no scene left, end
        if (scenes.empty()) {
            isRunning = false;
            break;
        }

        // Update & Render
        scenes.updateTop(dt, input);

        graphics.clear(0, 0, 0, 255);
        scenes.renderAll(graphics);
        graphics.present();
    }
}

void Engine::quit() {
    // Ensure scenes are properly exited before SDL quits
    scenes.clear(*this);

    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    renderer = nullptr;
    window = nullptr;

    SDL_Quit();
}
