#pragma once
#include <SDL2/SDL.h>

#include "Texture.h"
#include "Sprite.h"
#include "Camera2D.h"

class Graphics {
public:
    Graphics() : renderer(nullptr) {}

    void setRenderer(SDL_Renderer* r) { renderer = r; }
    SDL_Renderer* getRenderer() { return renderer; }
    SDL_Renderer* getRenderer() const { return renderer; }

    void setViewport(int w, int h) { viewportW = w; viewportH = h; }
    int getViewportW() const { return viewportW; }
    int getViewportH() const { return viewportH; }

    Camera2D& getCamera() { return camera; }
    const Camera2D& getCamera() const { return camera; }

    SDL_FPoint worldToScreen(float wx, float wy) const;
    SDL_FPoint screenToWorld(float sx, float sy) const;

    void clear(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

    void drawRect(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

    void drawRectWorld(float worldX, float worldY, float w, float h, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

    // ---- Texture ----
    void drawTextureScreen(
        const Texture& tex,
        int x, int y,
        int w = -1, int h = -1,
        const SDL_Rect* src = nullptr,
        double rotation = 0.0,
        SDL_RendererFlip flip = SDL_FLIP_NONE,
        SDL_Point origin = {-1,-1}
    );

    void drawTextureWorld(
        const Texture& tex,
        float worldX, float worldY,
        float scale = 1.0f,
        const SDL_Rect* src = nullptr,
        double rotation = 0.0,
        SDL_RendererFlip flip = SDL_FLIP_NONE,
        SDL_Point origin = {-1,-1}
    );

    // ---- Sprite ----
    void drawSpriteScreen(
        const Sprite& sp,
        float x, float y,
        float scale = 1.0f,
        double rotation = 0.0,
        SDL_RendererFlip flip = SDL_FLIP_NONE
    );

    void drawSpriteWorld(
        const Sprite& sp,
        float worldX, float worldY,
        float scale = 1.0f,
        double rotation = 0.0,
        SDL_RendererFlip flip = SDL_FLIP_NONE
    );

    void present();

private:
    SDL_Renderer* renderer;

    int viewportW = 800;
    int viewportH = 600;

    Camera2D camera;

private:
    bool isOffscreen(int x, int y, int w, int h) const;
};
