#pragma once
#include <SDL2/SDL.h>
#include <string>

class Texture {
public:
    Texture() = default;
    ~Texture() { destroy(); }

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&& other) noexcept { moveFrom(other); }
    Texture& operator=(Texture&& other) noexcept {
        if (this != &other) {
            destroy();
            moveFrom(other);
        }
        return *this;
    }

    bool load(SDL_Renderer* renderer, const std::string& path);
    void destroy();

    SDL_Texture* sdl() const { return tex; }
    int width() const { return w; }
    int height() const { return h; }
    bool valid() const { return tex != nullptr; }

private:
    SDL_Texture* tex = nullptr;
    int w = 0;
    int h = 0;

    void moveFrom(Texture& other) noexcept {
        tex = other.tex; w = other.w; h = other.h;
        other.tex = nullptr; other.w = 0; other.h = 0;
    }
};
