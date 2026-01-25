#include "../Engine/Core/Engine.h"
#include "../Engine/World/World.h"
#include "../Engine/Physics/Physics.h"
#include "../Engine/Entity/EntityManager.h"
#include "../Engine/Entity/Player.h"

class MainScene : public Scene {
public:
    void onEnter(Engine& engine) override {
        (void)engine;

        world.generateFlat(20, 15, 11);

        playerId = entities.create<Player>(120, 120);
    }

    void update(float dt, Input& input) override {
        entities.updateAll(dt, input, world);

        entities.forEachAlive([&](Entity& e){
            physics.apply(e, dt, world);
        });

        entities.sweepDead();
    }

    void render(Graphics& g) override {
        world.render(g);
        entities.renderAll(g);
    }

private:
    World world;
    Physics physics;
    EntityManager entities;
    EntityId playerId = NullEntity;
};

int main() {
    Engine::run<MainScene>("Rex Platformer", 800, 600);
    return 0;
}
