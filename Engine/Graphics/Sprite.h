#pragma once
#include <SDL2/SDL.h>
#include "Texture.h"

struct Sprite {
    const Texture* texture = nullptr;
    SDL_Rect src{0, 0, 0, 0};

    SDL_Point origin{0, 0};

    Sprite() = default;

    explicit Sprite(const Texture* tex) { setTexture(tex); }

    Sprite(const Texture* tex, SDL_Rect s, SDL_Point o = {-1, -1}) {
        texture = tex;
        src = s;
        setOrigin(o);
    }

    void setTexture(const Texture* tex) {
        texture = tex;
        if (texture && texture->valid()) {
            src = {0, 0, texture->width(), texture->height()};
            origin = { src.w / 2, src.h / 2 };
        } else {
            src = {0,0,0,0};
            origin = {0,0};
        }
    }

    void setSourceRect(SDL_Rect s) {
        src = s;
    }

    void setOrigin(SDL_Point o) {
        if (o.x < 0 && o.y < 0) {
            origin = { src.w / 2, src.h / 2 };
        } else {
            origin = o;
        }
    }

    bool valid() const { return texture && texture->valid() && src.w > 0 && src.h > 0; }
};
