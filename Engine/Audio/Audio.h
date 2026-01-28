#pragma once
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <algorithm>

class Audio {
public:
    Audio() = default;
    ~Audio() { quit(); }

    Audio(const Audio&) = delete;
    Audio& operator=(const Audio&) = delete;

    // SDL_mixer 전역 초기화/종료 (ref-count)
    bool init(
        int frequency = 44100,
        Uint16 format = MIX_DEFAULT_FORMAT,
        int channels = 2,
        int chunkSize = 2048,
        int allocateChannels = 32,
        int initFlags = (MIX_INIT_OGG | MIX_INIT_MP3)
    );

    void quit();

    bool ready() const;

    int  playSound(Mix_Chunk* chunk, int loops = 0, int channel = -1, int volume = -1);
    void haltChannel(int channel);
    void haltAllSounds();

    void setMasterSoundVolume(int volume); // 0~128
    int  getMasterSoundVolume() const;

    // ---- Music (Mix_Music*) ----
    bool playMusic(Mix_Music* music, int loops = -1, int volume = -1);
    void pauseMusic();
    void resumeMusic();
    void stopMusic();

    void setMusicVolume(int volume); // 0~128
    int  getMusicVolume() const;

private:
    static int clampVol(int v) { return std::clamp(v, 0, 128); }
};
