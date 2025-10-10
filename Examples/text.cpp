#define SDL_MAIN_HANDLED
#include "../Engine/Core/Engine.h"
#include "../Engine/Text/Text.h"

class TextDemo : public Game {
    Text text;

public:
    TextDemo() {
        text.init("assets/fonts/rex_engine.ttf", 32);
    }

    void update(float dt, Input& input) override {}

    void render(Graphics& g) override {
        g.clear(0, 0, 0, 255);
        text.render(g, "Hello, Rex Engine!", 200, 250, {255, 255, 255, 255});
        g.present();
    }

    ~TextDemo() {
        text.quit();
    }
};

int main() {
    Engine::run<TextDemo>("Text Test", 800, 600);
}
