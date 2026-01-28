#include "Texture.h"
#include <iostream>

#if __has_include(<SDL2/SDL_image.h>)
    #include <SDL2/SDL_image.h>
    #define REX_HAS_SDL_IMAGE 1
#else
    #define REX_HAS_SDL_IMAGE 0
#endif

namespace {
#if REX_HAS_SDL_IMAGE
    int g_img_refs = 0;
#endif
}

bool Texture::load(SDL_Renderer* renderer, const std::string& path) {
    destroy();
    if (!renderer) return false;

    SDL_Surface* surface = nullptr;
    SDL_QueryTexture(tex, nullptr, nullptr, &w, &h);

#if REX_HAS_SDL_IMAGE
    if (g_img_refs++ == 0) {
        int required = IMG_INIT_PNG;
        int optional = IMG_INIT_JPG;
        int inited = IMG_Init(required | optional);

        if ((inited & required) != required) {
            std::cerr << "IMG_Init Error (need PNG): " << IMG_GetError() << std::endl;
            g_img_refs--;
            IMG_Quit();
            return false;
        }
    }

    surface = IMG_Load(path.c_str());
    if (!surface) {
        std::cerr << "IMG_Load failed: " << path << " (" << IMG_GetError() << ")\n";
        if (--g_img_refs == 0) IMG_Quit();
        return false;
    }
#else
    surface = SDL_LoadBMP(path.c_str());
    if (!surface) {
        std::cerr << "SDL_LoadBMP failed: " << path << " (" << SDL_GetError() << ")\n";
        return false;
    }
#endif

    tex = SDL_CreateTextureFromSurface(renderer, surface);
    if (!tex) {
        std::cerr << "SDL_CreateTextureFromSurface failed: " << SDL_GetError() << "\n";
        SDL_FreeSurface(surface);
#if REX_HAS_SDL_IMAGE
        if (--g_img_refs == 0) IMG_Quit();
#endif
        return false;
    }

    w = surface->w;
    h = surface->h;
    SDL_FreeSurface(surface);

    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
    return true;
}

void Texture::destroy() {
    if (tex) {
        SDL_DestroyTexture(tex);
        tex = nullptr;
        w = 0; h = 0;
#if REX_HAS_SDL_IMAGE
        if (g_img_refs > 0 && --g_img_refs == 0) {
            IMG_Quit();
        }
#endif
    }
}
