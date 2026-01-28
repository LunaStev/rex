#define SDL_MAIN_HANDLED
#include "../Engine/Core/Engine.h"
#include "../Engine/Lua/LuaScene.h"

int main() {
    Engine::run<LuaScene>("Rex Lua Game", 800, 600, "Examples/scripts/main.lua");
    return 0;
}
