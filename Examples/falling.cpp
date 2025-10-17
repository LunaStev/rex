#define SDL_MAIN_HANDLED
#include "../Engine/Core/Engine.h"
#include "../Engine/Text/Text.h"
#include <vector>
#include <cstdlib>
#include <ctime>

struct Cube {
    float x, y, size;
    float speed;
    bool active;
};

class FallingCubesGame : public Game {
    float playerX = 380.0f;
    float playerY = 540.0f;
    float playerSpeed = 350.0f;
    float playerSize = 40.0f;

    std::vector<Cube> cubes;
    float spawnTimer = 0.0f;
    float spawnInterval = 0.8f;
    int score = 0;
    bool gameOver = false;

public:
    FallingCubesGame() {
        std::srand((unsigned)time(nullptr));
    }

    void update(float dt, Input& input) override {
        if (gameOver) {
            if (input.isKeyHeld(RexKey::RETURN)) {
                reset();
            }
            return;
        }

        // --- 플레이어 이동 ---
        if (input.isKeyHeld(RexKey::N1)) playerX -= playerSpeed * dt;
        if (input.isKeyHeld(RexKey::N2)) playerX += playerSpeed * dt;

        if (playerX < 0) playerX = 0;
        if (playerX + playerSize > 800) playerX = 800 - playerSize;

        // --- 큐브 생성 ---
        spawnTimer += dt;
        if (spawnTimer >= spawnInterval) {
            spawnTimer = 0.0f;
            spawnCube();
            if (spawnInterval > 0.3f)
                spawnInterval -= 0.01f; // 점점 빠르게
        }

        // --- 큐브 이동 ---
        for (auto& c : cubes) {
            if (!c.active) continue;
            c.y += c.speed * dt;

            // 충돌 체크
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

        // 플레이어
        g.drawRect((int)playerX, (int)playerY, (int)playerSize, (int)playerSize,
                   60, 200, 255, 255);

        // 큐브들
        for (auto& c : cubes) {
            if (c.active)
                g.drawRect((int)c.x, (int)c.y, (int)c.size, (int)c.size,
                           255, 100, 100, 255);
        }

        // UI
        SDL_Color white = {255, 255, 255, 255};
        static Text text;
        static bool initialized = false;
        if (!initialized) {
            text.init("assets/fonts/rex_engine.ttf", 28);
            initialized = true;
        }

        char buf[128];
        sprintf(buf, "Score: %d", score);
        text.render(g, buf, 20, 20, white);

        if (gameOver) {
            SDL_Color red = {255, 80, 80, 255};
            text.render(g, "GAME OVER", 300, 250, red);
            text.render(g, "Press ENTER to Restart", 250, 300, white);
        }

        g.present();
    }

private:
    void spawnCube() {
        Cube c;
        c.size = 30 + (rand() % 30);
        c.x = (float)(rand() % (800 - (int)c.size));
        c.y = -c.size;
        c.speed = 150 + (rand() % 150);
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
}
