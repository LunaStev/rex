#define SDL_MAIN_HANDLED
#include "../Engine/Core/Engine.h"
#include "../Engine/World/World.h"

class WorldTest : public Game {
    World world;

public:
    WorldTest() {
        world.generateFlat(20, 15, 10);
    }

    void update(float dt, Input& input) override {
        // world is fixed, no update
    }

    void render(Graphics& g) override {
        g.clear(50, 100, 200, 255);
        world.render(g);
        g.present();
    }
};

int main() {
    Engine::run<WorldTest>("World Test", 800, 600);
}
