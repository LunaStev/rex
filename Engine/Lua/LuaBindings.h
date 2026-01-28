#pragma once
#include "LuaVM.h"

class Engine;
class LuaScene;

namespace RexLua {
    struct Ctx {
        Engine* engine;
        LuaScene* scene;
    };

    void install(lua_State* L, Ctx* ctx);
}