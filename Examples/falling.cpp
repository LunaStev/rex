#define SDL_MAIN_HANDLED
#include "../Engine/Core/Engine.h"
#include "../Engine/Core/Scene.h"
#include "../Engine/Text/Text.h"

#include <vector>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <algorithm>

struct Cube {
    float x, y, size;
    float speed;
    bool active;
};

class FallingCubesGame : public Scene {
    float playerX = 380.0f;
    float playerY = 540.0f;
    float playerSpeed = 350.0f;
    float playerSize = 40.0f;

    std::vector<Cube> cubes;
    float spawnTimer = 0.0f;
    float spawnInterval = 0.8f;
    int score = 0;
    bool gameOver = false;

    Text text;

public:
    FallingCubesGame() {
        std::srand((unsigned)std::time(nullptr));
    }

    void onEnter(Engine& engine) override {
        text.init(engine.getAssets(), "assets/fonts/rex_engine.ttf", 28);
    }

    void update(float dt, Input& input) override {
        dt = std::min(dt, 0.05f);

        if (gameOver) {
            if (input.isKeyPressed(RexKey::RETURN)) reset();
            return;
        }

        if (input.isKeyHeld(RexKey::N1)) playerX -= playerSpeed * dt;
        if (input.isKeyHeld(RexKey::N2)) playerX += playerSpeed * dt;

        if (playerX < 0) playerX = 0;
        if (playerX + playerSize > 800) playerX = 800 - playerSize;

        spawnTimer += dt;
        if (spawnTimer >= spawnInterval) {
            spawnTimer = 0.0f;
            spawnCube();
            if (spawnInterval > 0.3f) spawnInterval -= 0.01f;
        }

        for (auto& c : cubes) {
            if (!c.active) continue;
            c.y += c.speed * dt;

            if (c.y + c.size >= playerY && c.y <= playerY + playerSize &&
                c.x + c.size >= playerX && c.x <= playerX + playerSize) {
                gameOver = true;
            }

            if (c.y > 600) {
                c.active = false;
                score++;
            }
        }
    }

    void render(Graphics& g) override {
        g.clear(10, 10, 30, 255);

        g.drawRect((int)playerX, (int)playerY, (int)playerSize, (int)playerSize,
                   60, 200, 255, 255);

        for (auto& c : cubes) {
            if (c.active) {
                g.drawRect((int)c.x, (int)c.y, (int)c.size, (int)c.size,
                           255, 100, 100, 255);
            }
        }

        SDL_Color white = {255, 255, 255, 255};

        char buf[128];
        std::snprintf(buf, sizeof(buf), "Score: %d", score);
        text.render(g, buf, 20, 20, white);

        if (gameOver) {
            SDL_Color red = {255, 80, 80, 255};
            text.render(g, "GAME OVER", 300, 250, red);
            text.render(g, "Press ENTER to Restart", 250, 300, white);
        }

        g.present();
    }

    ~FallingCubesGame() override {
        text.quit();
    }

private:
    void spawnCube() {
        Cube c;
        c.size = 30 + (std::rand() % 30);
        c.x = (float)(std::rand() % (800 - (int)c.size));
        c.y = -c.size;
        c.speed = 150 + (std::rand() % 150);
        c.active = true;
        cubes.push_back(c);
    }

    void reset() {
        cubes.clear();
        spawnTimer = 0.0f;
        spawnInterval = 0.8f;
        score = 0;
        gameOver = false;
        playerX = 380.0f;
    }
};

int main() {
    Engine::run<FallingCubesGame>("Falling Cubes - Rex Engine", 800, 600);
    return 0;
}
