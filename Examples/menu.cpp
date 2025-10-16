#define SDL_MAIN_HANDLED
#include "../Engine/Core/Engine.h"
#include <vector>
#include <cstdlib>
#include <string>

struct Tile {
    int type;
};

struct Item {
    std::string name;
    int quantity;
};

enum class GameState {
    MAIN_MENU,
    IN_GAME,
    INVENTORY
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

    GameState state = GameState::MAIN_MENU;
    std::vector<Item> inventory;
    int selectedItem = 0;

public:
    WorldDemo() {
        generateWorld();
        generateInventory();
    }

    void generateWorld() {
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

    void generateInventory() {
        inventory = {
            {"Pickaxe", 1},
            {"Stone", 12},
            {"Wood", 7},
            {"Torch", 3},
        };
    }

    void update(float dt, Input& input) override {
        switch (state) {
            case GameState::MAIN_MENU:
                if (input.isKeyHeld(RexKey::RETURN)) {
                    state = GameState::IN_GAME;
                }
                break;

            case GameState::IN_GAME:
                updateWorld(dt, input);
                if (input.isKeyHeld(RexKey::I)) {
                    state = GameState::INVENTORY;
                }
                break;

            case GameState::INVENTORY:
                if (input.isKeyHeld(RexKey::ESCAPE) || input.isKeyHeld(RexKey::I)) {
                    state = GameState::IN_GAME;
                }
                break;
        }
    }

    void updateWorld(float dt, Input& input) {
        if (input.isKeyHeld(RexKey::A)) playerX -= moveSpeed * dt;
        if (input.isKeyHeld(RexKey::D)) playerX += moveSpeed * dt;

        if (isGrounded && input.isKeyHeld(RexKey::SPACE)) {
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
        g.clear(40, 80, 160, 255);

        switch (state) {
            case GameState::MAIN_MENU:
                renderMenu(g);
                break;
            case GameState::IN_GAME:
                renderWorld(g);
                break;
            case GameState::INVENTORY:
                renderWorld(g);
                renderInventory(g);
                break;
        }

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

    void renderWorld(Graphics& g) {
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
    }

    void renderMenu(Graphics& g) {
        g.drawRect(0, 0, 800, 600, 0, 0, 0, 200);
        g.drawRect(250, 200, 300, 80, 255, 255, 255, 255);
        g.drawRect(250, 320, 300, 80, 200, 0, 0, 255);
    }

    void renderInventory(Graphics& g) {
        g.drawRect(100, 100, 600, 400, 0, 0, 0, 180);
        int y = 130;
        for (size_t i = 0; i < inventory.size(); i++) {
            int highlight = (i == selectedItem ? 60 : 0);
            g.drawRect(120, y, 560, 40, 100 + highlight, 100 + highlight, 100 + highlight, 255);
            y += 50;
        }
    }
};

int main() {
    Engine::run<WorldDemo>("Rex Engine - Menu & Inventory", 800, 600);
}
