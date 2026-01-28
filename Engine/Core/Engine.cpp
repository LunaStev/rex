#include "Engine.h"
#include <algorithm>

Engine::Engine() = default;
Engine::~Engine() { quit(); }

bool Engine::init(const char* title, int width, int height) {
    quit(); // 재초기화 안전

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL Init Error: " << SDL_GetError() << "\n";
        return false;
    }
    sdlInited = true;

    windowW = width;
    windowH = height;

    window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    if (!window) {
        std::cerr << "Window Error: " << SDL_GetError() << "\n";
        quit();
        return false;
    }

    renderer = SDL_CreateRenderer(
        window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (!renderer) {
        std::cerr << "Renderer Error: " << SDL_GetError() << "\n";
        quit();
        return false;
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    graphics.setRenderer(renderer);
    graphics.setViewport(width, height);

    assets.setRenderer(renderer);

    if (!audio.init()) {
        std::cerr << "Audio init failed. Continuing without audio.\n";
        // 오디오는 “필수”로 두고 싶으면 여기서 quit()+return false로 바꿔
    }

    isRunning = true;
    return true;
}

void Engine::syncViewportIfNeeded() {
    if (!window) return;

    int w = 0, h = 0;
    SDL_GetWindowSize(window, &w, &h);
    if (w <= 0 || h <= 0) return;

    if (w != windowW || h != windowH) {
        windowW = w;
        windowH = h;
        graphics.setViewport(w, h);
    }
}

void Engine::mainLoop() {
    const Uint64 freq = SDL_GetPerformanceFrequency();
    Uint64 last = SDL_GetPerformanceCounter();

    double acc = 0.0;
    const double maxFrame = 0.25;
    const int maxStepsPerFrame = 5;

    while (isRunning) {
        input.update(isRunning);
        syncViewportIfNeeded();

        Uint64 now = SDL_GetPerformanceCounter();
        double frame = (double)(now - last) / (double)freq;
        last = now;

        if (frame > maxFrame) frame = maxFrame;
        acc += frame;

        scenes.applyPending(*this);
        if (scenes.empty()) { isRunning = false; break; }

        int steps = 0;
        while (acc >= FIXED_DT && steps < maxStepsPerFrame) {
            scenes.applyPending(*this);
            if (scenes.empty()) { isRunning = false; break; }

            scenes.updateTop((float)FIXED_DT, input);
            acc -= FIXED_DT;
            steps++;
        }

        // 너무 느린 프레임에서 "죽음의 누적" 방지
        if (steps == maxStepsPerFrame) {
            acc = 0.0;
        }

        if (!isRunning) break;

        graphics.clear(0, 0, 0, 255);
        scenes.renderAll(graphics);
        graphics.present();
    }
}

void Engine::quit() {
    // 항상 멱등성(idempotent) 보장
    isRunning = false;

    // 씬 훅이 assets/audio를 쓸 수 있으니, 먼저 씬부터 정리
    scenes.clear(*this);

    assets.clear();
    audio.quit();

    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    graphics.setRenderer(nullptr);
    assets.setRenderer(nullptr);

    if (sdlInited) {
        SDL_Quit();
        sdlInited = false;
    }
}
