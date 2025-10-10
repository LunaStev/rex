#define SDL_MAIN_HANDLED
#include "../Engine/Core/Engine.h"
#include "../Engine/World/World.h"
#include "../Engine/Entity/Entity.h"
#include "../Engine/Physics/Physics.h"

class PhysicsTest : public Game {
    World world;
    Entity player;
    Physics physics;
    bool grounded = false;

public:
    PhysicsTest() {
        world.generateFlat(20, 15, 10);
        player.setPosition(200, 200);
        player.setColor(255, 255, 255);
    }

    void update(float dt, Input& input) override {
        if (input.isKeyHeld(SDL_SCANCODE_A)) player.setVX(-250);
        else if (input.isKeyHeld(SDL_SCANCODE_D)) player.setVX(250);
        else player.setVX(0);

        if (grounded && input.isKeyHeld(SDL_SCANCODE_SPACE)) {
            player.setVY(-500);
            grounded = false;
        }

        physics.apply(player, dt, world);
        grounded = physics.checkGroundCollision(player, world);
    }

    void render(Graphics& g) override {
        g.clear(20, 20, 40, 255);
        world.render(g);
        player.render(g);
        g.present();
    }
};

int main() {
    Engine::run<PhysicsTest>("Physics Test", 800, 600);
}
