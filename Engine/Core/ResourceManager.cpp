#include "ResourceManager.h"
#include <iostream>

namespace {
    int  g_ttf_refs  = 0;     // ResourceManager 인스턴스 기준 ref
    bool g_ttf_owned = false; // 우리가 TTF_Init 했는지 여부
}

bool ResourceManager::ensureTTF() {
    if (holdsTtfRef) return true;

    holdsTtfRef = true;

    if (g_ttf_refs++ == 0) {
        if (TTF_WasInit() == 0) {
            if (TTF_Init() == -1) {
                holdsTtfRef = false;
                g_ttf_refs--;
                std::cerr << "TTF_Init Error: " << TTF_GetError() << "\n";
                return false;
            }
            g_ttf_owned = true;
        }
    }

    return true;
}

bool ResourceManager::ensureMixerReady() {
    int freq = 0, channels = 0;
    Uint16 fmt = 0;
    if (Mix_QuerySpec(&freq, &fmt, &channels) == 0) {
        std::cerr << "SDL_mixer not ready (Mix_OpenAudio not called). Call Audio::init() first.\n";
        return false;
    }
    return true;
}

// ---------------- Texture ----------------
Texture* ResourceManager::getTexture(const std::string& path) {
    auto it = texturesByPath.find(path);
    if (it != texturesByPath.end()) return it->second.get();

    if (!renderer) {
        std::cerr << "ResourceManager: renderer is null (setRenderer first)\n";
        return nullptr;
    }

    auto tex = std::make_unique<Texture>();
    if (!tex->load(renderer, path)) {
        return nullptr;
    }

    Texture* ret = tex.get();
    texturesByPath.emplace(path, std::move(tex));
    return ret;
}

bool ResourceManager::addTexture(const std::string& name, const std::string& path) {
    Texture* t = getTexture(path);
    if (!t) return false;
    textureAlias[name] = path;
    return true;
}

Texture* ResourceManager::texture(const std::string& nameOrPath) {
    auto it = textureAlias.find(nameOrPath);
    if (it != textureAlias.end()) return getTexture(it->second);
    return getTexture(nameOrPath);
}

// ---------------- Font ----------------
TTF_Font* ResourceManager::getFont(const std::string& path, int size) {
    if (!ensureTTF()) return nullptr;

    FontKey key{path, size};
    auto it = fonts.find(key);
    if (it != fonts.end()) return it->second.get();

    std::unique_ptr<TTF_Font, FontDeleter> f(TTF_OpenFont(path.c_str(), size));
    if (!f) {
        std::cerr << "Failed to load font: " << path << " (" << TTF_GetError() << ")\n";
        return nullptr;
    }

    TTF_Font* ret = f.get();
    fonts.emplace(std::move(key), std::move(f));
    return ret;
}

bool ResourceManager::addFont(const std::string& name, const std::string& path, int size) {
    TTF_Font* f = getFont(path, size);
    if (!f) return false;
    fontAlias[name] = FontAliasInfo{path, size};
    return true;
}

TTF_Font* ResourceManager::font(const std::string& name) {
    auto it = fontAlias.find(name);
    if (it == fontAlias.end()) return nullptr;
    return getFont(it->second.path, it->second.size);
}

// ---------------- Sound / Music ----------------
Mix_Chunk* ResourceManager::getSound(const std::string& path) {
    auto it = soundsByPath.find(path);
    if (it != soundsByPath.end()) return it->second.get();

    if (!ensureMixerReady()) return nullptr;

    std::unique_ptr<Mix_Chunk, ChunkDeleter> c(Mix_LoadWAV(path.c_str()));
    if (!c) {
        std::cerr << "Failed to load sound: " << path << " (" << Mix_GetError() << ")\n";
        return nullptr;
    }

    Mix_Chunk* ret = c.get();
    soundsByPath.emplace(path, std::move(c));
    return ret;
}

Mix_Music* ResourceManager::getMusic(const std::string& path) {
    auto it = musicByPath.find(path);
    if (it != musicByPath.end()) return it->second.get();

    if (!ensureMixerReady()) return nullptr;

    std::unique_ptr<Mix_Music, MusicDeleter> m(Mix_LoadMUS(path.c_str()));
    if (!m) {
        std::cerr << "Failed to load music: " << path << " (" << Mix_GetError() << ")\n";
        return nullptr;
    }

    Mix_Music* ret = m.get();
    musicByPath.emplace(path, std::move(m));
    return ret;
}

bool ResourceManager::addSound(const std::string& name, const std::string& path) {
    Mix_Chunk* c = getSound(path);
    if (!c) return false;
    soundAlias[name] = path;
    return true;
}

bool ResourceManager::addMusic(const std::string& name, const std::string& path) {
    Mix_Music* m = getMusic(path);
    if (!m) return false;
    musicAlias[name] = path;
    return true;
}

Mix_Chunk* ResourceManager::sound(const std::string& nameOrPath) {
    auto it = soundAlias.find(nameOrPath);
    if (it != soundAlias.end()) return getSound(it->second);
    return getSound(nameOrPath);
}

Mix_Music* ResourceManager::music(const std::string& nameOrPath) {
    auto it = musicAlias.find(nameOrPath);
    if (it != musicAlias.end()) return getMusic(it->second);
    return getMusic(nameOrPath);
}

void ResourceManager::clear() {
    texturesByPath.clear();
    textureAlias.clear();

    fonts.clear();
    fontAlias.clear();

    soundsByPath.clear();
    musicByPath.clear();
    soundAlias.clear();
    musicAlias.clear();

    if (holdsTtfRef) {
        holdsTtfRef = false;

        if (g_ttf_refs > 0 && --g_ttf_refs == 0) {
            if (g_ttf_owned) {
                TTF_Quit();
                g_ttf_owned = false;
            }
        }
    }
}
