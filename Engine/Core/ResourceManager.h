#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include <string>
#include <unordered_map>
#include <memory>
#include <cstdint>

#include "../Graphics/Texture.h"

// ---- Deleters ----
struct FontDeleter {
    void operator()(TTF_Font* f) const { if (f) TTF_CloseFont(f); }
};
struct ChunkDeleter {
    void operator()(Mix_Chunk* c) const { if (c) Mix_FreeChunk(c); }
};
struct MusicDeleter {
    void operator()(Mix_Music* m) const { if (m) Mix_FreeMusic(m); }
};

class ResourceManager {
public:
    ResourceManager() = default;
    ~ResourceManager() { clear(); }

    void setRenderer(SDL_Renderer* r) { renderer = r; }
    SDL_Renderer* getRenderer() const { return renderer; }

    // ---- Texture ----
    Texture* getTexture(const std::string& path);
    bool addTexture(const std::string& name, const std::string& path);
    Texture* texture(const std::string& nameOrPath);

    // ---- Font ----
    TTF_Font* getFont(const std::string& path, int size);
    bool addFont(const std::string& name, const std::string& path, int size);
    TTF_Font* font(const std::string& name);

    // ---- Sound / Music ----
    Mix_Chunk* getSound(const std::string& path);
    Mix_Music* getMusic(const std::string& path);

    bool addSound(const std::string& name, const std::string& path);
    bool addMusic(const std::string& name, const std::string& path);

    Mix_Chunk* sound(const std::string& nameOrPath);
    Mix_Music* music(const std::string& nameOrPath);

    void clear();

private:
    SDL_Renderer* renderer = nullptr;

    // ---- textures ----
    std::unordered_map<std::string, std::unique_ptr<Texture>> texturesByPath;
    std::unordered_map<std::string, std::string> textureAlias;

    // ---- fonts ----
    struct FontKey {
        std::string path;
        int size;
        bool operator==(const FontKey& o) const { return size == o.size && path == o.path; }
    };
    struct FontKeyHash {
        std::size_t operator()(const FontKey& k) const noexcept {
            return std::hash<std::string>()(k.path) ^ (std::hash<int>()(k.size) + 0x9e3779b9);
        }
    };

    std::unordered_map<FontKey, std::unique_ptr<TTF_Font, FontDeleter>, FontKeyHash> fonts;
    struct FontAliasInfo { std::string path; int size; };
    std::unordered_map<std::string, FontAliasInfo> fontAlias;

    // ---- sounds/music ----
    std::unordered_map<std::string, std::unique_ptr<Mix_Chunk, ChunkDeleter>> soundsByPath;
    std::unordered_map<std::string, std::unique_ptr<Mix_Music, MusicDeleter>> musicByPath;

    std::unordered_map<std::string, std::string> soundAlias;
    std::unordered_map<std::string, std::string> musicAlias;

    // 이 인스턴스가 TTF “ref”를 잡고 있는지(중복 clear 방지)
    bool holdsTtfRef = false;

private:
    bool ensureTTF();
    bool ensureMixerReady();
};
