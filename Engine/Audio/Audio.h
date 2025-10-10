#pragma once
#include <SDL2/SDL_mixer.h>
#include <string>
#include <iostream>
#include <unordered_map>

class Audio {
private:
    Mix_Music* currentMusic = nullptr;
    std::unordered_map<std::string, Mix_Chunk*> soundEffects;

public:
    Audio();
    ~Audio();

    bool init();

    bool loadMusic(const std::string& path);
    void playMusic(int loops = -1);
    void pauseMusic();
    void resumeMusic();
    void stopMusic();

    bool loadSound(const std::string& name, const std::string& path);
    void playSound(const std::string& name, int loops = 0);

    void setMusicVolume(int volume); // 0~128
    void setSoundVolume(const std::string& name, int volume);

    void quit();
};
