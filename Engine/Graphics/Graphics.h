#pragma once
#include <SDL2/SDL.h>

class Graphics {
public:
    Graphics() : renderer(nullptr) {}

    void setRenderer(SDL_Renderer* r) { renderer = r; }
    void clear(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
    void drawRect(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
    void present();

private:
    SDL_Renderer* renderer;
};