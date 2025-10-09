#define SDL_MAIN_HANDLED
#include "../Engine/Core/Engine.h"
#include <vector>
#include <cstdlib>

struct Brick {
    int x, y, w, h;
    bool destroyed = false;
};

class BreakoutGame : public Game {
    float paddleX = 350.0f;
    float paddleY = 550.0f;
    float paddleSpeed = 400.0f;

    float ballX = 390.0f;
    float ballY = 300.0f;
    float ballDX = 1.0f;
    float ballDY = -1.0f;
    float ballSpeed = 300.0f;

    int lives = 3;
    std::vector<Brick> bricks;

public:
    BreakoutGame() {
        resetBricks();
    }

    void resetBricks() {
        bricks.clear();
        int cols = 10;
        int rows = 5;
        int brickWidth = 70;
        int brickHeight = 25;
        int offsetX = 20;
        int offsetY = 40;

        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                Brick b;
                b.x = offsetX + c * (brickWidth + 5);
                b.y = offsetY + r * (brickHeight + 5);
                b.w = brickWidth;
                b.h = brickHeight;
                bricks.push_back(b);
            }
        }
    }

    void resetBall() {
        ballX = 390;
        ballY = 300;
        ballDX = (rand() % 2 == 0 ? 1 : -1);
        ballDY = -1;
    }

    void update(float dt, Input& input) override {
        if (input.isKeyHeld(SDL_SCANCODE_LEFT)) paddleX -= paddleSpeed * dt;
        if (input.isKeyHeld(SDL_SCANCODE_RIGHT)) paddleX += paddleSpeed * dt;

        if (paddleX < 0) paddleX = 0;
        if (paddleX + 100 > 800) paddleX = 700;

        ballX += ballDX * ballSpeed * dt;
        ballY += ballDY * ballSpeed * dt;

        if (ballX <= 0 || ballX + 20 >= 800) ballDX *= -1;
        if (ballY <= 0) ballDY *= -1;

        if (ballY + 20 >= paddleY && ballX + 20 >= paddleX && ballX <= paddleX + 100) {
            ballDY *= -1;
            ballY = paddleY - 20;
        }

        for (auto& b : bricks) {
            if (!b.destroyed && ballX + 20 > b.x && ballX < b.x + b.w && ballY + 20 > b.y && ballY < b.y + b.h) {
                b.destroyed = true;
                ballDY *= -1;
                break;
            }
        }

        if (ballY > 600) {
            lives--;
            resetBall();
            if (lives <= 0) {
                resetBricks();
                lives = 3;
            }
        }
    }

    void render(Graphics& g) override {
        g.drawRect((int)paddleX, (int)paddleY, 100, 20, 255, 255, 255, 255);
        g.drawRect((int)ballX, (int)ballY, 20, 20, 255, 200, 0, 255);
        for (auto& b : bricks) {
            if (!b.destroyed)
                g.drawRect(b.x, b.y, b.w, b.h, 100 + rand() % 155, 100 + rand() % 155, 255, 255);
        }
    }
};

int main() {
    Engine::run<BreakoutGame>("Rex Breakout", 800, 600);
}
