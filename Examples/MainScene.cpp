#include "../Engine/Core/Engine.h"
#include "MainScene.h"

int main() {
    Engine::run<MainScene>("Rex", 1280, 720);
    return 0;
}
