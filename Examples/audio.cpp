#define SDL_MAIN_HANDLED
#include "../Engine/Core/Engine.h"
#include "../Engine/Core/Scene.h"
#include "../Engine/Audio/Audio.h"

#include <SDL2/SDL_mixer.h>
#include <cstdio>

class AudioDemo : public Scene {
    Audio audio;

    Mix_Music* bgm = nullptr;
    Mix_Chunk* jump = nullptr;

    bool musicPlaying = false;

public:
    void onEnter(Engine& engine) override {
        (void)engine;

        audio.init();

        bgm = Mix_LoadMUS("assets/bgm.ogg");
        if (!bgm) {
            std::fprintf(stderr, "Mix_LoadMUS failed: %s\n", Mix_GetError());
        }

        jump = Mix_LoadWAV("assets/jump.wav");
        if (!jump) {
            std::fprintf(stderr, "Mix_LoadWAV failed: %s\n", Mix_GetError());
        }
    }

    void update(float dt, Input& input) override {
        (void)dt;

        if (jump && input.isKeyPressed(RexKey::SPACE)) {
            audio.playSound(jump);
        }

        if (input.isKeyPressed(RexKey::M)) {
            if (!musicPlaying) {
                if (bgm) audio.playMusic(bgm);
                musicPlaying = true;
            } else {
                Mix_HaltMusic();
                musicPlaying = false;
            }
        }
    }

    void render(Graphics& g) override {
        g.clear(0, 0, 0, 255);
        g.present();
    }

    ~AudioDemo() override {
        if (jump) Mix_FreeChunk(jump);
        if (bgm) Mix_FreeMusic(bgm);
        audio.quit();
    }
};

int main() {
    Engine::run<AudioDemo>("Audio Test", 800, 600);
    return 0;
}
