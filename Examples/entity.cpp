#define SDL_MAIN_HANDLED
#include "../Engine/Core/Engine.h"
#include "../Engine/Entity/Entity.h"

class EntityTest : public Game {
    Entity box;

public:
    EntityTest() {
        box.setPosition(200, 200);
        box.setColor(255, 0, 0);
    }

    void update(float dt, Input& input) override {
        float speed = 300 * dt;
        if (input.isKeyHeld(RexKey::RIGHT)) box.setPosition(box.getX() + speed, box.getY());
        if (input.isKeyHeld(RexKey::LEFT))  box.setPosition(box.getX() - speed, box.getY());
        if (input.isKeyHeld(RexKey::UP))    box.setPosition(box.getX(), box.getY() - speed);
        if (input.isKeyHeld(RexKey::DOWN))  box.setPosition(box.getX(), box.getY() + speed);

        if (input.isKeyHeld(RexKey::R)) box.setColor(255, 0, 0);
        if (input.isKeyHeld(RexKey::G)) box.setColor(0, 255, 0);
        if (input.isKeyHeld(RexKey::B)) box.setColor(0, 0, 255);
    }

    void render(Graphics& g) override {
        g.clear(30, 30, 30, 255);
        box.render(g);
        g.present();
    }
};

int main() {
    Engine::run<EntityTest>("Entity Test", 800, 600);
}
