#include "Audio.h"

Audio::Audio() {}
Audio::~Audio() { quit(); }

bool Audio::init() {
    int flags = MIX_INIT_OGG | MIX_INIT_MP3;
    int inited = Mix_Init(flags);
    if ((inited & flags) != flags) {
        std::cerr << "SDL_mixer Mix_Init Error: " << Mix_GetError() << std::endl;
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer OpenAudio Error: " << Mix_GetError() << std::endl;
        Mix_Quit();
        return false;
    }

    Mix_AllocateChannels(32);
    return true;
}

bool Audio::loadMusic(const std::string& path) {
    currentMusic = Mix_LoadMUS(path.c_str());
    if (!currentMusic) {
        std::cerr << "Failed to load music: " << Mix_GetError() << std::endl;
        return false;
    }
    return true;
}

void Audio::playMusic(int loops) {
    if (currentMusic) Mix_PlayMusic(currentMusic, loops);
}

void Audio::pauseMusic() { Mix_PauseMusic(); }
void Audio::resumeMusic() { Mix_ResumeMusic(); }
void Audio::stopMusic() { Mix_HaltMusic(); }

bool Audio::loadSound(const std::string& name, const std::string& path) {
    Mix_Chunk* chunk = Mix_LoadWAV(path.c_str());
    if (!chunk) {
        std::cerr << "Failed to load sound: " << path << " (" << Mix_GetError() << ")" << std::endl;
        return false;
    }
    soundEffects[name] = chunk;
    return true;
}

void Audio::playSound(const std::string& name, int loops) {
    auto it = soundEffects.find(name);
    if (it != soundEffects.end()) {
        Mix_PlayChannel(-1, it->second, loops);
    }
}

void Audio::setMusicVolume(int volume) {
    Mix_VolumeMusic(volume);
}

void Audio::setSoundVolume(const std::string& name, int volume) {
    auto it = soundEffects.find(name);
    if (it != soundEffects.end()) {
        Mix_VolumeChunk(it->second, volume);
    }
}

void Audio::quit() {
    if (currentMusic) {
        Mix_FreeMusic(currentMusic);
        currentMusic = nullptr;
    }

    for (auto& [_, chunk] : soundEffects) {
        Mix_FreeChunk(chunk);
    }
    soundEffects.clear();

    Mix_CloseAudio();
    Mix_Quit();
}
