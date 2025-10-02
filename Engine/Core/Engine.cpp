#include "Engine.h"

Engine::Engine() : window(nullptr), renderer(nullptr), isRunning(false) {}

Engine::~Engine() {
    quit();
}

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
        std::cerr << "Rednerer Error: " << SDL_GetError() << std::endl;
        return false;
    }

    isRunning = true;
    return true;
}

void Engine::run() {
    SDL_Event e;
    int x = 100, y = 100;

    while (isRunning) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                isRunning = false;
            }
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) {
                    isRunning = false;
                }
            }
        }

        const Uint8* state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_RIGHT]) x += 2;
        if (state[SDL_SCANCODE_LEFT]) x -= 2;
        if (state[SDL_SCANCODE_DOWN]) y += 2;
        if (state[SDL_SCANCODE_UP]) y -= 2;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_Rect rect = {x, y, 50, 50};
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &rect);

        SDL_RenderPresent(renderer);

        SDL_Delay(16); // ~60FPS
    }
}

void Engine::quit() {
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}