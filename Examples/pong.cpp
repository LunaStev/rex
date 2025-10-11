#define SDL_MAIN_HANDLED
#include "../Engine/Core/Engine.h"

class PongGame : public Game {
    float paddleSpeed = 400.0f;
    float ballSpeed = 300.0f;
    float paddleWidth = 20.0f, paddleHeight = 100.0f;

    float leftY = 250.0f;
    float rightY = 250.0f;
    float ballX = 390.0f, ballY = 290.0f;
    float ballDX = 1.0f, ballDY = 1.0f;

    int leftScore = 0, rightScore = 0;

public:
    void update(float dt, Input& input) override {
        if (input.isKeyHeld(RexKey::W)) leftY -= paddleSpeed * dt;
        if (input.isKeyHeld(RexKey::S)) leftY += paddleSpeed * dt;

        if (input.isKeyHeld(RexKey::UP)) rightY -= paddleSpeed * dt;
        if (input.isKeyHeld(RexKey::DOWN)) rightY += paddleSpeed * dt;

        ballX += ballDX * ballSpeed * dt;
        ballY += ballDY * ballSpeed * dt;

        if (ballY <= 0 || ballY + 20 >= 600) ballDY *= -1;

        if (ballX <= 40 && ballY + 20 >= leftY && ballY <= leftY + paddleHeight) {
            ballDX *= -1;
            ballX = 40;
        }
        if (ballX + 20 >= 760 && ballY + 20 >= rightY && ballY <= rightY + paddleHeight) {
            ballDX *= -1;
            ballX = 740;
        }

        if (ballX < 0) {
            rightScore++;
            resetBall(-1);
        } else if (ballX > 800) {
            leftScore++;
            resetBall(1);
        }

        if (leftY < 0) leftY = 0;
        if (leftY + paddleHeight > 600) leftY = 600 - paddleHeight;
        if (rightY < 0) rightY = 0;
        if (rightY + paddleHeight > 600) rightY = 600 - paddleHeight;
    }

    void render(Graphics& g) override {
        g.drawRect(395, 0, 10, 600, 255, 255, 255, 100);

        g.drawRect(20, (int)leftY, (int)paddleWidth, (int)paddleHeight, 255, 255, 255, 255);
        g.drawRect(760, (int)rightY, (int)paddleWidth, (int)paddleHeight, 255, 255, 255, 255);

        g.drawRect((int)ballX, (int)ballY, 20, 20, 255, 255, 255, 255);
    }

private:
    void resetBall(int dir) {
        ballX = 390.0f;
        ballY = 290.0f;
        ballDX = (dir > 0 ? 1.0f : -1.0f);
        ballDY = (rand() % 2 == 0 ? 1.0f : -1.0f);
    }
};

int main() {
    Engine::run<PongGame>("Pong - Rex Engine", 800, 600);
}
