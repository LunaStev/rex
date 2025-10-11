#define SDL_MAIN_HANDLED
#include "../Engine/Core/Engine.h"

class MyGame : public Game {
    float x = 100, y = 100;
public:
    void update(float dt, Input& input) override {
        if (input.isKeyHeld(RexKey::RIGHT)) x += 200 * dt;
        if (input.isKeyHeld(RexKey::LEFT))  x -= 200 * dt;
        if (input.isKeyHeld(RexKey::DOWN))  y += 200 * dt;
        if (input.isKeyHeld(RexKey::UP))    y -= 200 * dt;
    }

    void render(Graphics& g) override {
        g.drawRect((int)x, (int)y, 50, 50, 255, 0, 0, 255);
    }
};

int main() {
    Engine::run<MyGame>("Rex Engine Game", 800, 600);
}
