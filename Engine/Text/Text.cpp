#include "Text.h"

#include "../Graphics/Graphics.h"
#include "../Core/ResourceManager.h"

namespace {
    int  g_ttf_refs = 0;
    bool g_ttf_owned = false; // Text가 TTF_Init 한 경우에만 true
}

bool Text::acquireTtf() {
    if (g_ttf_refs++ == 0) {
        if (TTF_WasInit() == 0) {
            if (TTF_Init() == -1) {
                std::cerr << "TTF_Init Error: " << TTF_GetError() << "\n";
                g_ttf_refs--;
                return false;
            }
            g_ttf_owned = true;
        } else {
            // 이미 외부에서 init됨 -> Text는 quit 책임 없음
            g_ttf_owned = false;
        }
    }
    return true;
}

void Text::releaseTtf() {
    if (g_ttf_refs <= 0) return;
    if (--g_ttf_refs == 0) {
        if (g_ttf_owned) {
            TTF_Quit();
        }
        g_ttf_owned = false;
    }
}

void Text::setCacheLimit(size_t n) {
    cacheLimit = (n < 16 ? 16 : n);
    cache.reserve(cacheLimit);
    evictIfNeeded();
}

bool Text::init(ResourceManager& assets, const std::string& fontPath, int fontSize) {
    quit();

    font = assets.getFont(fontPath, fontSize);
    ownsFont = false;
    holdsTtfRef = false;       // ResourceManager가 책임
    cachedRenderer = nullptr;  // renderer는 render 시점에 잡음

    if (!font) {
        std::cerr << "Text::init failed (assets.getFont): " << fontPath << "\n";
        return false;
    }

    cache.reserve(cacheLimit);
    return true;
}

bool Text::initRaw(const std::string& fontPath, int fontSize) {
    quit();

    if (!acquireTtf()) return false;
    holdsTtfRef = true;
    cachedRenderer = nullptr;

    font = TTF_OpenFont(fontPath.c_str(), fontSize);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << "\n";

        // acquire 했으니 실패 시 release
        holdsTtfRef = false;
        releaseTtf();
        return false;
    }

    ownsFont = true;
    cache.reserve(cacheLimit);
    return true;
}

void Text::clearCache() {
    for (auto& [k, e] : cache) {
        if (e.tex) SDL_DestroyTexture(e.tex);
    }
    cache.clear();
    lru.clear();
}

Text::Entry* Text::getOrCreate(Graphics& g, const std::string& str, SDL_Color color) {
    if (!font) return nullptr;

    SDL_Renderer* renderer = g.getRenderer();
    if (!renderer) return nullptr;

    // renderer가 바뀌면 기존 캐시 텍스처는 전부 무효
    if (cachedRenderer != renderer) {
        clearCache();
        cachedRenderer = renderer;
    }

    Key key{str, pack(color)};
    auto it = cache.find(key);
    if (it != cache.end()) {
        // LRU move-to-front (복사 없음)
        lru.splice(lru.begin(), lru, it->second.it);
        it->second.it = lru.begin();
        return &it->second;
    }

    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, str.c_str(), color);
    if (!surface) return nullptr;

    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
    if (!tex) {
        SDL_FreeSurface(surface);
        return nullptr;
    }

    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);

    Entry e;
    e.tex = tex;
    e.w = surface->w;
    e.h = surface->h;

    SDL_FreeSurface(surface);

    // insert + LRU
    lru.push_front(key);
    e.it = lru.begin();

    auto [newIt, ok] = cache.emplace(std::move(key), e);
    (void)ok;

    evictIfNeeded();
    return &newIt->second;
}

void Text::evictIfNeeded() {
    while (cache.size() > cacheLimit && !lru.empty()) {
        Key old = std::move(lru.back());
        lru.pop_back();

        auto it = cache.find(old);
        if (it != cache.end()) {
            if (it->second.tex) SDL_DestroyTexture(it->second.tex);
            cache.erase(it);
        }
    }
}

void Text::render(Graphics& g, const std::string& str, int x, int y, SDL_Color color) {
    Entry* e = getOrCreate(g, str, color);
    if (!e || !e->tex) return;

    SDL_Rect dst = {x, y, e->w, e->h};
    SDL_RenderCopy(g.getRenderer(), e->tex, nullptr, &dst);
}

bool Text::measure(const std::string& text, int& outW, int& outH) const {
    outW = 0;
    outH = 0;
    if (!font) return false;

    if (TTF_SizeUTF8(font, text.c_str(), &outW, &outH) != 0) {
        return false;
    }
    return true;
}

void Text::quit() {
    clearCache();
    cachedRenderer = nullptr;

    if (ownsFont && font) {
        TTF_CloseFont(font);
        font = nullptr;
        ownsFont = false;
    } else {
        font = nullptr;
        ownsFont = false;
    }

    if (holdsTtfRef) {
        holdsTtfRef = false;
        releaseTtf();
    }
}
