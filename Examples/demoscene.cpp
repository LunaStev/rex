#include "../Engine/Core/Scene.h"
#include "../Engine/World/World.h"
#include "../Engine/Entity/EntityManager.h"
#include "../Engine/Entity/Player.h"
#include "../Engine/Physics/Physics.h"
#include "../Engine/Core/Engine.h"

class DemoScene : public Scene {
public:
    void onEnter(Engine& engine) override;
    void update(float dt, Input& input) override;
    void render(Graphics& g) override;

private:
    World world;
    EntityManager entities;
    Physics physics;
    EntityId playerId = NullEntity;
};

void DemoScene::onEnter(Engine& engine) {
    (void)engine;

    world.generateFlat(40, 20, 12);

    for (int x = 6; x <= 11; ++x) world.setTile(x, 9, 1);
    for (int x = 15; x <= 18; ++x) world.setTile(x, 7, 2);
    for (int y = 6; y <= 11; ++y) world.setTile(25, y, 1);

    playerId = entities.create<Player>(120, 120);
}

void DemoScene::update(float dt, Input& input) {
    entities.updateAll(dt, input, world);

    entities.forEachAlive([&](Entity& e) {
        physics.apply(e, dt, world);
    });

    entities.sweepDead();
}

void DemoScene::render(Graphics& g) {
    world.render(g);
    entities.renderAll(g);
}

int main() {
    Engine::run<DemoScene>("Rex Demo", 800, 600);
    return 0;
}