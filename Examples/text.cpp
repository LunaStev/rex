#define SDL_MAIN_HANDLED
#include "../Engine/Core/Engine.h"
#include "../Engine/Core/Scene.h"
#include "../Engine/Text/Text.h"

class TextDemo : public Scene {
    Text text;

public:
    void onEnter(Engine& engine) override {
        text.init(engine.getAssets(), "assets/fonts/rex_engine.ttf", 32);
    }

    void update(float dt, Input& input) override {
        (void)dt; (void)input;
    }

    void render(Graphics& g) override {
        g.clear(0, 0, 0, 255);
        text.render(g, "Hello, Rex Engine!", 200, 250, {255, 255, 255, 255});
        g.present();
    }

    ~TextDemo() override {
        text.quit();
    }
};

int main() {
    Engine::run<TextDemo>("Text Test", 800, 600);
    return 0;
}
