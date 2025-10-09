#define SDL_MAIN_HANDLED
#include "../Engine/Core/Engine.h"
#include <vector>
#include <cstdlib>

struct Tile {
    int type; // 0: empty, 1: ground, 2: grass
};

class WorldDemo : public Game {
    static const int TILE_SIZE = 40;
    static const int WORLD_WIDTH = 20;
    static const int WORLD_HEIGHT = 15;

    std::vector<Tile> world;
    float playerX = 200;
    float playerY = 200;
    float velocityY = 0;
    bool isGrounded = false;

    float moveSpeed = 250;
    float gravity = 900;

public:
    WorldDemo() {
        world.resize(WORLD_WIDTH * WORLD_HEIGHT);
        for (int y = 0; y < WORLD_HEIGHT; y++) {
            for (int x = 0; x < WORLD_WIDTH; x++) {
                Tile& t = world[y * WORLD_WIDTH + x];
                if (y > 10) t.type = 1;
                else if (y == 10) t.type = 2;
                else t.type = 0;
            }
        }
    }

    void update(float dt, Input& input) override {
        if (input.isKeyHeld(SDL_SCANCODE_A)) playerX -= moveSpeed * dt;
        if (input.isKeyHeld(SDL_SCANCODE_D)) playerX += moveSpeed * dt;

        if (isGrounded && input.isKeyHeld(SDL_SCANCODE_SPACE)) {
            velocityY = -500;
            isGrounded = false;
        }

        velocityY += gravity * dt;
        playerY += velocityY * dt;

        float playerBottom = playerY + 40;
        float groundY = findGroundBelow(playerX + 20, playerBottom);
        if (playerBottom >= groundY) {
            playerY = groundY - 40;
            velocityY = 0;
            isGrounded = true;
        } else {
            isGrounded = false;
        }

        if (playerX < 0) playerX = 0;
        if (playerX + 40 > WORLD_WIDTH * TILE_SIZE)
            playerX = WORLD_WIDTH * TILE_SIZE - 40;
    }

    void render(Graphics& g) override {
        g.clear(50, 100, 200, 255);

        for (int y = 0; y < WORLD_HEIGHT; y++) {
            for (int x = 0; x < WORLD_WIDTH; x++) {
                Tile& t = world[y * WORLD_WIDTH + x];
                int screenX = x * TILE_SIZE;
                int screenY = y * TILE_SIZE;
                if (t.type == 1)
                    g.drawRect(screenX, screenY, TILE_SIZE, TILE_SIZE, 90, 60, 30, 255);
                else if (t.type == 2)
                    g.drawRect(screenX, screenY, TILE_SIZE, TILE_SIZE, 30, 200, 60, 255);
            }
        }

        g.drawRect((int)playerX, (int)playerY, 40, 40, 255, 255, 255, 255);

        g.present();
    }

private:
    float findGroundBelow(float x, float yBottom) {
        for (int y = 0; y < WORLD_HEIGHT; y++) {
            int idx = y * WORLD_WIDTH + (int)(x / TILE_SIZE);
            if (world[idx].type != 0) {
                return y * TILE_SIZE;
            }
        }
        return WORLD_HEIGHT * TILE_SIZE;
    }
};

int main() {
    Engine::run<WorldDemo>("Rex World Demo", 800, 600);
}
