#include "Audio.h"

namespace {
    struct MixerGlobal {
        int refs = 0;
        bool ok = false;
        bool opened = false;
        int initFlagsWanted = 0;
        int initFlagsGot = 0;
        int allocateChannels = 32;
    } g;
}

bool Audio::init(int frequency, Uint16 format, int channels, int chunkSize, int allocateChannels, int initFlags) {
    // 이미 초기화된 경우: ref만 증가
    if (g.refs > 0) {
        g.refs++;
        return g.ok;
    }

    g.refs = 1;
    g.ok = false;
    g.opened = false;
    g.initFlagsWanted = initFlags;
    g.allocateChannels = allocateChannels;

    int inited = Mix_Init(initFlags);
    g.initFlagsGot = inited;

    if (inited == 0) {
        std::cerr << "SDL_mixer Mix_Init failed: " << Mix_GetError() << "\n";
        g.refs = 0;
        return false;
    }

    if ((inited & initFlags) != initFlags) {
        std::cerr << "SDL_mixer warning: some codecs not available. wanted="
                  << initFlags << " got=" << inited
                  << " (" << Mix_GetError() << ")\n";
    }

    // 2) open audio
    if (Mix_OpenAudio(frequency, format, channels, chunkSize) < 0) {
        std::cerr << "SDL_mixer Mix_OpenAudio failed: " << Mix_GetError() << "\n";
        Mix_Quit();
        g.refs = 0;
        return false;
    }
    g.opened = true;

    // 3) channels
    Mix_AllocateChannels(allocateChannels);

    Mix_VolumeMusic(128);
    Mix_Volume(-1, 128);

    g.ok = true;
    return true;
}

void Audio::quit() {
    if (g.refs == 0) return;
    if (--g.refs > 0) return;

    if (g.opened) {
        Mix_HaltMusic();
        Mix_HaltChannel(-1);
        Mix_CloseAudio();
        g.opened = false;
    }

    Mix_Quit();

    g.ok = false;
    g.initFlagsWanted = 0;
    g.initFlagsGot = 0;
    g.allocateChannels = 32;
}

bool Audio::ready() const {
    if (!g.ok) return false;
    int freq = 0, ch = 0;
    Uint16 fmt = 0;
    return Mix_QuerySpec(&freq, &fmt, &ch) != 0;
}

// ---- Sound ----
int Audio::playSound(Mix_Chunk* chunk, int loops, int channel, int volume) {
    if (!ready() || !chunk) return -1;

    if (volume >= 0) {
        Mix_VolumeChunk(chunk, clampVol(volume));
    }

    int played = Mix_PlayChannel(channel, chunk, loops);
    if (played == -1) {
        std::cerr << "Mix_PlayChannel failed: " << Mix_GetError() << "\n";
    }
    return played;
}

void Audio::haltChannel(int channel) {
    if (!ready()) return;
    Mix_HaltChannel(channel);
}

void Audio::haltAllSounds() {
    if (!ready()) return;
    Mix_HaltChannel(-1);
}

void Audio::setMasterSoundVolume(int volume) {
    if (!ready()) return;
    Mix_Volume(-1, clampVol(volume));
}

int Audio::getMasterSoundVolume() const {
    if (!ready()) return 0;
    return Mix_Volume(-1, -1);
}

// ---- Music ----
bool Audio::playMusic(Mix_Music* music, int loops, int volume) {
    if (!ready() || !music) return false;

    if (volume >= 0) {
        Mix_VolumeMusic(clampVol(volume));
    }

    if (Mix_PlayMusic(music, loops) == -1) {
        std::cerr << "Mix_PlayMusic failed: " << Mix_GetError() << "\n";
        return false;
    }
    return true;
}

void Audio::pauseMusic()  { if (ready()) Mix_PauseMusic(); }
void Audio::resumeMusic() { if (ready()) Mix_ResumeMusic(); }
void Audio::stopMusic()   { if (ready()) Mix_HaltMusic(); }

void Audio::setMusicVolume(int volume) {
    if (!ready()) return;
    Mix_VolumeMusic(clampVol(volume));
}

int Audio::getMusicVolume() const {
    if (!ready()) return 0;
    return Mix_VolumeMusic(-1);
}
