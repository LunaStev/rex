#define SDL_MAIN_HANDLED
#include "../Engine/Core/Engine.h"
#include "../Engine/Text/Text.h"
#include "../Engine/Audio/Audio.h"
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <map>
#include <sstream>
#include <algorithm>

struct Cell {
    int x, y;
    bool wall;
};

struct Enemy {
    float x, y;
    int hp;
    bool alive = true;
};

class RogueGame : public Game {
    static constexpr int TILE = 32;
    static constexpr int MAP_W = 25;
    static constexpr int MAP_H = 18;

    std::vector<Cell> map;
    std::vector<Enemy> enemies;
    float playerX = 3 * TILE;
    float playerY = 3 * TILE;
    int playerHP = 100;
    int score = 0;
    bool gameOver = false;

    std::mt19937 rng;
    Text text;
    Audio audio;
    std::chrono::steady_clock::time_point startTime;

public:
    RogueGame() : rng(std::random_device{}()) {
        text.init("assets/fonts/rex_engine.ttf", 24);
        audio.init();
        audio.loadSound("hit", "assets/sounds/hit.wav");
        audio.loadSound("attack", "assets/sounds/attack.wav");
        audio.loadSound("dead", "assets/sounds/dead.wav");
        generateMap();
        spawnEnemies(8);
        startTime = std::chrono::steady_clock::now();
    }

    void update(float dt, Input& input) override {
        if (gameOver) {
            if (input.isKeyHeld(RexKey::RETURN)) reset();
            return;
        }

        float speed = 200.0f;
        float newX = playerX;
        float newY = playerY;

        if (input.isKeyHeld(RexKey::W)) newY -= speed * dt;
        if (input.isKeyHeld(RexKey::S)) newY += speed * dt;
        if (input.isKeyHeld(RexKey::A)) newX -= speed * dt;
        if (input.isKeyHeld(RexKey::D)) newX += speed * dt;

        if (!isWall(newX, newY)) {
            playerX = newX;
            playerY = newY;
        }

        // Attack
        if (input.isKeyPressed(RexKey::SPACE)) {
            audio.playSound("attack");
            for (auto& e : enemies) {
                if (!e.alive) continue;
                float dx = e.x - playerX;
                float dy = e.y - playerY;
                float dist = sqrt(dx * dx + dy * dy);
                if (dist < 60) {
                    e.hp -= 25;
                    if (e.hp <= 0) {
                        e.alive = false;
                        audio.playSound("dead");
                        score += 10;
                    } else {
                        audio.playSound("hit");
                    }
                }
            }
        }

        // Enemy Movement
        for (auto& e : enemies) {
            if (!e.alive) continue;
            float dx = playerX - e.x;
            float dy = playerY - e.y;
            float len = sqrt(dx * dx + dy * dy);
            if (len > 0) {
                dx /= len;
                dy /= len;
                e.x += dx * 80 * dt;
                e.y += dy * 80 * dt;
            }

            // Hit in the envent of a collision
            if (abs(e.x - playerX) < 20 && abs(e.y - playerY) < 20) {
                playerHP -= 10;
                if (playerHP <= 0) {
                    gameOver = true;
                    audio.playSound("dead");
                }
            }
        }
    }

    void render(Graphics& g) override {
        g.clear(15, 15, 30, 255);

        // --- Map Render ---
        for (auto& c : map) {
            if (c.wall)
                g.drawRect(c.x * TILE, c.y * TILE, TILE, TILE, 50, 50, 50, 255);
            else
                g.drawRect(c.x * TILE, c.y * TILE, TILE, TILE, 30, 30, 60, 255);
        }

        // --- Enemy ---
        for (auto& e : enemies) {
            if (!e.alive) continue;
            g.drawRect((int)e.x, (int)e.y, 20, 20, 255, 60, 60, 255);
        }

        // --- Player ---
        g.drawRect((int)playerX, (int)playerY, 24, 24, 100, 255, 180, 255);

        // --- UI ---
        SDL_Color white = {255, 255, 255, 255};
        SDL_Color red = {255, 100, 100, 255};
        SDL_Color yellow = {255, 255, 100, 255};

        text.render(g, "HP: " + std::to_string(playerHP), 10, 10, red);
        text.render(g, "Score: " + std::to_string(score), 10, 40, yellow);

        auto now = std::chrono::steady_clock::now();
        auto sec = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
        text.render(g, "Time: " + std::to_string(sec), 10, 70, white);

        if (gameOver) {
            SDL_Color gray = {200, 200, 200, 255};
            text.render(g, "GAME OVER", 300, 260, gray);
            text.render(g, "Press ENTER to Restart", 220, 300, gray);
        }

        g.present();
    }

    ~RogueGame() {
        text.quit();
        audio.quit();
    }

private:
    void generateMap() {
        map.clear();
        map.reserve(MAP_W * MAP_H);

        std::uniform_int_distribution<int> randWall(0, 100);
        for (int y = 0; y < MAP_H; ++y) {
            for (int x = 0; x < MAP_W; ++x) {
                Cell c {x, y, false};
                if (x == 0 || y == 0 || x == MAP_W - 1 || y == MAP_H - 1)
                    c.wall = true;
                else if (randWall(rng) < 15)
                    c.wall = true;
                map.push_back(c);
            }
        }
    }

    void spawnEnemies(int count) {
        enemies.clear();
        std::uniform_int_distribution<int> distX(2, MAP_W - 3);
        std::uniform_int_distribution<int> distY(2, MAP_H - 3);
        for (int i = 0; i < count; ++i) {
            Enemy e;
            e.x = distX(rng) * TILE;
            e.y = distY(rng) * TILE;
            e.hp = 50 + (rand() % 50);
            enemies.push_back(e);
        }
    }

    bool isWall(float x, float y) {
        int tx = (int)(x / TILE);
        int ty = (int)(y / TILE);
        if (tx < 0 || ty < 0 || tx >= MAP_W || ty >= MAP_H) return true;
        return map[ty * MAP_W + tx].wall;
    }

    void reset() {
        playerHP = 100;
        score = 0;
        playerX = 3 * TILE;
        playerY = 3 * TILE;
        generateMap();
        spawnEnemies(8);
        gameOver = false;
        startTime = std::chrono::steady_clock::now();
    }
};

int main() {
    Engine::run<RogueGame>("Rex Rogue: Dark Cavern", 800, 600);
}
