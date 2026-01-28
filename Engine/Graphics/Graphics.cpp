#include "Graphics.h"
#include <cmath>

SDL_FPoint Graphics::worldToScreen(float wx, float wy) const {
    const float z = camera.zoom;
    return SDL_FPoint{
        (wx - camera.x) * z + (viewportW * 0.5f),
        (wy - camera.y) * z + (viewportH * 0.5f),
    };
}

SDL_FPoint Graphics::screenToWorld(float sx, float sy) const {
    const float z = camera.zoom;
    return SDL_FPoint{
        (sx - (viewportW * 0.5f)) / z + camera.x,
        (sy - (viewportH * 0.5f)) / z + camera.y,
    };
}

bool Graphics::isOffscreen(int x, int y, int w, int h) const {
    if (w <= 0 || h <= 0) return true;
    if (x + w < 0) return true;
    if (y + h < 0) return true;
    if (x > viewportW) return true;
    if (y > viewportH) return true;
    return false;
}

void Graphics::clear(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    if (!renderer) return;
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderClear(renderer);
}

void Graphics::drawRect(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    if (!renderer) return;
    SDL_Rect rect = {x, y, w, h};
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderFillRect(renderer, &rect);
}

void Graphics::drawRectWorld(float worldX, float worldY, float w, float h, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    if (!renderer) return;

    const float z = camera.zoom;
    SDL_FPoint s = worldToScreen(worldX, worldY);

    int rx = (int)std::lround(s.x);
    int ry = (int)std::lround(s.y);
    int rw = (int)std::lround(w * z);
    int rh = (int)std::lround(h * z);

    if (isOffscreen(rx, ry, rw, rh)) return;

    SDL_Rect rect{ rx, ry, rw, rh };
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderFillRect(renderer, &rect);
}

void Graphics::drawTextureScreen(
    const Texture& tex,
    int x, int y,
    int w, int h,
    const SDL_Rect* src,
    double rotation,
    SDL_RendererFlip flip,
    SDL_Point origin
) {
    if (!renderer || !tex.valid()) return;

    SDL_Rect s = src ? *src : SDL_Rect{0, 0, tex.width(), tex.height()};

    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    dst.w = (w < 0) ? s.w : w;
    dst.h = (h < 0) ? s.h : h;

    if (isOffscreen(dst.x, dst.y, dst.w, dst.h)) return;

    SDL_Point o = origin;
    if (o.x < 0 && o.y < 0) o = { dst.w / 2, dst.h / 2 };

    SDL_RenderCopyEx(renderer, tex.sdl(), &s, &dst, rotation, &o, flip);
}

void Graphics::drawTextureWorld(
    const Texture& tex,
    float worldX, float worldY,
    float scale,
    const SDL_Rect* src,
    double rotation,
    SDL_RendererFlip flip,
    SDL_Point origin
) {
    if (!renderer || !tex.valid()) return;

    SDL_Rect s = src ? *src : SDL_Rect{0, 0, tex.width(), tex.height()};

    const float z = camera.zoom;
    SDL_FPoint p = worldToScreen(worldX, worldY);

    const float finalScale = scale * z;

    int dw = (int)std::lround(s.w * finalScale);
    int dh = (int)std::lround(s.h * finalScale);

    SDL_Point o = origin;
    if (o.x < 0 && o.y < 0) {
        o = { dw / 2, dh / 2 };
    } else {
        o.x = (int)std::lround(o.x * finalScale);
        o.y = (int)std::lround(o.y * finalScale);
    }

    int dx = (int)std::lround(p.x - o.x);
    int dy = (int)std::lround(p.y - o.y);

    if (isOffscreen(dx, dy, dw, dh)) return;

    SDL_Rect dst{ dx, dy, dw, dh };
    SDL_RenderCopyEx(renderer, tex.sdl(), &s, &dst, rotation, &o, flip);
}

void Graphics::drawSpriteScreen(
    const Sprite& sp,
    float x, float y,
    float scale,
    double rotation,
    SDL_RendererFlip flip
) {
    if (!renderer || !sp.valid()) return;

    int dw = (int)std::lround(sp.src.w * scale);
    int dh = (int)std::lround(sp.src.h * scale);

    int ox = (int)std::lround(sp.origin.x * scale);
    int oy = (int)std::lround(sp.origin.y * scale);

    int dx = (int)std::lround(x - ox);
    int dy = (int)std::lround(y - oy);

    if (isOffscreen(dx, dy, dw, dh)) return;

    SDL_Rect dst{ dx, dy, dw, dh };
    SDL_Point o{ ox, oy };

    SDL_RenderCopyEx(renderer, sp.texture->sdl(), &sp.src, &dst, rotation, &o, flip);
}

void Graphics::drawSpriteWorld(
    const Sprite& sp,
    float worldX, float worldY,
    float scale,
    double rotation,
    SDL_RendererFlip flip
) {
    if (!renderer || !sp.valid()) return;

    const float z = camera.zoom;
    SDL_FPoint p = worldToScreen(worldX, worldY);

    const float finalScale = scale * z;

    int dw = (int)std::lround(sp.src.w * finalScale);
    int dh = (int)std::lround(sp.src.h * finalScale);

    int ox = (int)std::lround(sp.origin.x * finalScale);
    int oy = (int)std::lround(sp.origin.y * finalScale);

    int dx = (int)std::lround(p.x - ox);
    int dy = (int)std::lround(p.y - oy);

    if (isOffscreen(dx, dy, dw, dh)) return;

    SDL_Rect dst{ dx, dy, dw, dh };
    SDL_Point o{ ox, oy };

    SDL_RenderCopyEx(renderer, sp.texture->sdl(), &sp.src, &dst, rotation, &o, flip);
}

void Graphics::present() {
    if (!renderer) return;
    SDL_RenderPresent(renderer);
}
