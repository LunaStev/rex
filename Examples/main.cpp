#include "../Engine/Core/Engine.h"

int main(int argc, char* argv[]) {
    Engine engine;
    if (!engine.init("My 2D Engine", 800, 600)) {
        return -1;
    }

    engine.run();
    return 0;
}