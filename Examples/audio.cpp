#define SDL_MAIN_HANDLED
#include "../Engine/Core/Engine.h"
#include "../Engine/Audio/Audio.h"

class AudioDemo : public Game {
    Audio audio;
    bool musicPlaying = false;

public:
    AudioDemo() {
        audio.init();
        audio.loadMusic("assets/bgm.ogg");
        audio.loadSound("jump", "assets/jump.wav");
    }

    void update(float dt, Input& input) override {
        if (input.isKeyHeld(SDL_SCANCODE_SPACE)) {
            audio.playSound("jump");
        }
        if (input.isKeyHeld(SDL_SCANCODE_M)) {
            if (!musicPlaying) {
                audio.playMusic();
                musicPlaying = true;
            } else {
                audio.stopMusic();
                musicPlaying = false;
            }
        }
    }

    void render(Graphics& g) override {
        g.clear(0, 0, 0, 255);
        g.present();
    }

    ~AudioDemo() {
        audio.quit();
    }
};

int main() {
    Engine::run<AudioDemo>("Audio Test", 800, 600);
}
