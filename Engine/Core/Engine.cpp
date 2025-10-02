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

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer Error: " << SDL_GetError() << std::endl;
        return false;
    }

    graphics.setRenderer(renderer);
    isRunning = true;
    return true;
}

void Engine::mainLoop(Game& game) {
    Uint32 lastTick = SDL_GetTicks();

    while (isRunning) {
        input.update(isRunning);

        // 델타 타임 계산
        Uint32 currentTick = SDL_GetTicks();
        float dt = (currentTick - lastTick) / 1000.0f;
        lastTick = currentTick;

        // 게임 로직
        game.update(dt, input);

        // 렌더링
        graphics.clear(0, 0, 0, 255);
        game.render(graphics);
        graphics.present();

        SDL_Delay(16); // ~60FPS
    }
}

void Engine::quit() {
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}
