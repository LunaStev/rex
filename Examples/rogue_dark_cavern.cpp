#define SDL_MAIN_HANDLED
#include "../Engine/Core/Engine.h"
#include "../Engine/Core/Scene.h"

#include "../Engine/Text/Text.h"
#include "../Engine/Audio/Audio.h"

#include "../Engine/World/World.h"
#include "../Engine/Entity/Entity.h"
#include "../Engine/Entity/EntityManager.h"
#include "../Engine/Physics/Physics.h"

#include <vector>
#include <random>
#include <chrono>
#include <string>
#include <cmath>
#include <algorithm>

// -------------------------
// Game Entities (local)
// -------------------------

class RoguePlayer final : public Entity {
public:
    float speed = 260.0f;

    RoguePlayer(float x, float y)
        : Entity(x, y, 24, 24) {
        setColor(100, 255, 180, 255);
    }

    void update(float dt, Input& input, World& world) override {
        (void)dt;
        (void)world;

        float dx = 0.0f;
        float dy = 0.0f;

        if (input.isKeyHeld(RexKey::W) || input.isKeyHeld(RexKey::UP))    dy -= 1.0f;
        if (input.isKeyHeld(RexKey::S) || input.isKeyHeld(RexKey::DOWN))  dy += 1.0f;
        if (input.isKeyHeld(RexKey::A) || input.isKeyHeld(RexKey::LEFT))  dx -= 1.0f;
        if (input.isKeyHeld(RexKey::D) || input.isKeyHeld(RexKey::RIGHT)) dx += 1.0f;

        float len = std::sqrt(dx*dx + dy*dy);
        if (len > 0.0f) {
            dx /= len;
            dy /= len;
        }

        setVelocity(dx * speed, dy * speed);
    }
};

class RogueEnemy final : public Entity {
public:
    int hp = 80;

    RogueEnemy(float x, float y, int hp)
        : Entity(x, y, 20, 20), hp(hp) {
        setColor(255, 60, 60, 255);
    }
};

// -------------------------
// Rogue Scene
// -------------------------

class RogueScene final : public Scene {
    static constexpr int SCREEN_W = 800;
    static constexpr int SCREEN_H = 600;

    static constexpr int MAP_W = SCREEN_W / World::TILE_SIZE; // 20
    static constexpr int MAP_H = SCREEN_H / World::TILE_SIZE; // 15

    World world;
    EntityManager em;
    Physics physics;

    EntityId playerId = NullEntity;
    std::vector<EntityId> enemyIds;

    int playerHP = 100;
    int score = 0;
    bool gameOver = false;

    float hitCooldown = 0.0f;
    float attackCooldown = 0.0f;

    std::mt19937 rng{ std::random_device{}() };

    Text text;
    Audio audio;
    std::chrono::steady_clock::time_point startTime;

public:
    RogueScene() { }

    void onEnter(Engine& engine) override {
        (void)engine;

        physics.gravity = 0.0f;

        // Text/Audio init
        text.init("assets/fonts/rex_engine.ttf", 24);

        audio.init();
        audio.loadSound("hit", "assets/sounds/hit.wav");
        audio.loadSound("attack", "assets/sounds/attack.wav");
        audio.loadSound("dead", "assets/sounds/dead.wav");

        reset();
    }

    void update(float dt, Input& input) override {
        dt = std::min(dt, 0.05f);

        if (hitCooldown > 0.0f) hitCooldown -= dt;
        if (attackCooldown > 0.0f) attackCooldown -= dt;

        if (gameOver) {
            if (input.isKeyPressed(RexKey::RETURN)) reset();
            return;
        }

        em.updateAll(dt, input, world);

        Entity* p = em.get(playerId);
        if (p) {
            for (auto id : enemyIds) {
                Entity* base = em.get(id);
                if (!base || !base->isAlive()) continue;

                auto* e = dynamic_cast<RogueEnemy*>(base);
                if (!e) continue;

                float dx = (p->getX() + p->getWidth()*0.5f) - (e->getX() + e->getWidth()*0.5f);
                float dy = (p->getY() + p->getHeight()*0.5f) - (e->getY() + e->getHeight()*0.5f);
                float len = std::sqrt(dx*dx + dy*dy);
                if (len > 0.0001f) {
                    dx /= len;
                    dy /= len;
                }

                float enemySpeed = 110.0f;
                e->setVelocity(dx * enemySpeed, dy * enemySpeed);
            }
        }

        em.forEachAlive([&](Entity& e) {
            physics.apply(e, dt, world);
        });

        if (p && input.isKeyPressed(RexKey::SPACE) && attackCooldown <= 0.0f) {
            attackCooldown = 0.18f;
            audio.playSound("attack");

            float px = p->getX() + p->getWidth()*0.5f;
            float py = p->getY() + p->getHeight()*0.5f;
            float attackRange = 70.0f;

            for (auto id : enemyIds) {
                Entity* base = em.get(id);
                if (!base || !base->isAlive()) continue;

                auto* e = dynamic_cast<RogueEnemy*>(base);
                if (!e) continue;

                float ex = e->getX() + e->getWidth()*0.5f;
                float ey = e->getY() + e->getHeight()*0.5f;

                float dx = ex - px;
                float dy = ey - py;
                float dist = std::sqrt(dx*dx + dy*dy);

                if (dist <= attackRange) {
                    e->hp -= 25;
                    if (e->hp <= 0) {
                        e->kill();
                        audio.playSound("dead");
                        score += 10;
                    } else {
                        audio.playSound("hit");

                        if (dist > 0.0001f) {
                            dx /= dist;
                            dy /= dist;
                            e->setVelocity(dx * 240.0f, dy * 240.0f);
                        }
                    }
                }
            }
        }

        if (p) {
            for (auto id : enemyIds) {
                Entity* base = em.get(id);
                if (!base || !base->isAlive()) continue;

                if (Physics::isColliding(*p, *base)) {
                    if (hitCooldown <= 0.0f) {
                        hitCooldown = 0.45f;
                        playerHP -= 10;

                        if (playerHP <= 0) {
                            playerHP = 0;
                            gameOver = true;
                            audio.playSound("dead");
                        }
                    }
                }
            }
        }

        em.sweepDead();
    }

    void render(Graphics& g) override {
        for (int y = 0; y < world.height; ++y) {
            for (int x = 0; x < world.width; ++x) {
                int t = world.tiles[y * world.width + x].type;

                if (t == 1) {
                    // wall
                    g.drawRect(x * World::TILE_SIZE, y * World::TILE_SIZE,
                               World::TILE_SIZE, World::TILE_SIZE,
                               55, 55, 55, 255);
                } else {
                    // floor
                    g.drawRect(x * World::TILE_SIZE, y * World::TILE_SIZE,
                               World::TILE_SIZE, World::TILE_SIZE,
                               20, 20, 45, 255);
                }
            }
        }

        // --- Entities ---
        em.renderAll(g);

        // --- UI ---
        SDL_Color white  = {255, 255, 255, 255};
        SDL_Color red    = {255, 100, 100, 255};
        SDL_Color yellow = {255, 255, 100, 255};
        SDL_Color gray   = {200, 200, 200, 255};

        text.render(g, "HP: " + std::to_string(playerHP), 10, 10, red);
        text.render(g, "Score: " + std::to_string(score), 10, 40, yellow);

        auto now = std::chrono::steady_clock::now();
        auto sec = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
        text.render(g, "Time: " + std::to_string(sec), 10, 70, white);

        if (gameOver) {
            text.render(g, "GAME OVER", 300, 260, gray);
            text.render(g, "Press ENTER to Restart", 220, 300, gray);
        }
    }

private:
    void reset() {
        world.width = MAP_W;
        world.height = MAP_H;
        world.tiles.assign(world.width * world.height, Tile{0}); // floor

        generateMap();

        enemyIds.clear();
        em = EntityManager{};

        playerHP = 100;
        score = 0;
        gameOver = false;
        hitCooldown = 0.0f;
        attackCooldown = 0.0f;

        float px = 2.0f * World::TILE_SIZE;
        float py = 2.0f * World::TILE_SIZE;

        playerId = em.create<RoguePlayer>(px, py);

        spawnEnemies(8);

        startTime = std::chrono::steady_clock::now();
    }

    void generateMap() {
        std::uniform_int_distribution<int> randWall(0, 100);

        for (int y = 0; y < world.height; ++y) {
            for (int x = 0; x < world.width; ++x) {
                bool isBorder = (x == 0 || y == 0 || x == world.width - 1 || y == world.height - 1);
                bool wall = false;

                if (isBorder) wall = true;
                else if (randWall(rng) < 16) wall = true;

                world.tiles[y * world.width + x].type = wall ? 1 : 0;
            }
        }

        clearArea(1, 1, 4, 4);
    }

    void clearArea(int x0, int y0, int x1, int y1) {
        x0 = std::max(0, x0);
        y0 = std::max(0, y0);
        x1 = std::min(world.width - 1, x1);
        y1 = std::min(world.height - 1, y1);

        for (int y = y0; y <= y1; ++y) {
            for (int x = x0; x <= x1; ++x) {
                world.tiles[y * world.width + x].type = 0;
            }
        }
    }

    void spawnEnemies(int count) {
        std::uniform_int_distribution<int> distX(1, world.width - 2);
        std::uniform_int_distribution<int> distY(1, world.height - 2);
        std::uniform_int_distribution<int> distHP(50, 120);

        Entity* p = em.get(playerId);
        float px = p ? p->getX() : 0.0f;
        float py = p ? p->getY() : 0.0f;

        int tries = 0;
        while ((int)enemyIds.size() < count && tries < 5000) {
            ++tries;

            int tx = distX(rng);
            int ty = distY(rng);

            if (world.getTile(tx, ty) != 0) continue;

            float ex = tx * World::TILE_SIZE + 10.0f;
            float ey = ty * World::TILE_SIZE + 10.0f;

            float dx = ex - px;
            float dy = ey - py;
            if (std::sqrt(dx*dx + dy*dy) < 140.0f) continue;

            int hp = distHP(rng);
            EntityId id = em.create<RogueEnemy>(ex, ey, hp);
            enemyIds.push_back(id);
        }
    }
};

int main() {
    Engine::run<RogueScene>("Rex Rogue: Dark Cavern", 800, 600);
    return 0;
}
