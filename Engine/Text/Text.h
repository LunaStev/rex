#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <string>
#include <iostream>
#include <unordered_map>
#include <list>
#include <cstdint>
#include <utility>

class Graphics;
class ResourceManager;

class Text {
private:
    TTF_Font* font = nullptr;
    bool ownsFont = false;

    // initRaw()에서만 TTF 수명관리 (내가 init한 경우에만 quit)
    bool holdsTtfRef = false;

    // renderer 바뀌면 캐시 무효라서 추적
    SDL_Renderer* cachedRenderer = nullptr;

    struct Key {
        std::string text;
        uint32_t rgba; // packed
        bool operator==(const Key& o) const { return rgba == o.rgba && text == o.text; }
    };

    struct KeyHash {
        std::size_t operator()(const Key& k) const noexcept {
            // hash combine (괜찮은 믹싱)
            std::size_t h1 = std::hash<std::string>()(k.text);
            std::size_t h2 = std::hash<uint32_t>()(k.rgba);
            return h1 ^ (h2 + 0x9e3779b97f4a7c15ULL + (h1 << 6) + (h1 >> 2));
        }
    };

    struct Entry {
        SDL_Texture* tex = nullptr;
        int w = 0;
        int h = 0;
        std::list<Key>::iterator it; // LRU iterator
    };

    std::unordered_map<Key, Entry, KeyHash> cache;
    std::list<Key> lru; // front = most recent
    size_t cacheLimit = 256;

public:
    Text() = default;
    ~Text() { quit(); }

    Text(const Text&) = delete;
    Text& operator=(const Text&) = delete;
    Text(Text&&) = delete;
    Text& operator=(Text&&) = delete;

    // ResourceManager로 폰트 받기 (TTF 수명은 ResourceManager가 책임진다고 가정)
    bool init(ResourceManager& assets, const std::string& fontPath, int fontSize);

    // raw로 폰트 직접 열기 (Text가 TTF_Init/Quit까지 책임)
    bool initRaw(const std::string& fontPath, int fontSize);

    void setCacheLimit(size_t n);
    void clearCache();

    // 렌더 (screen 좌표)
    void render(Graphics& g, const std::string& text, int x, int y,
                SDL_Color color = {255, 255, 255, 255});

    // 크기만 측정 (렌더/캐시 없이)
    bool measure(const std::string& text, int& outW, int& outH) const;

    void quit();

private:
    static uint32_t pack(SDL_Color c) {
        return (uint32_t)c.r | ((uint32_t)c.g << 8) | ((uint32_t)c.b << 16) | ((uint32_t)c.a << 24);
    }

    Entry* getOrCreate(Graphics& g, const std::string& str, SDL_Color color);
    void evictIfNeeded();

    // initRaw용 TTF refcount
    static bool acquireTtf();
    static void releaseTtf();
};
