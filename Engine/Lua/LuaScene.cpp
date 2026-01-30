#include "LuaScene.h"
#include "../Core/Engine.h"
#include <iostream>

bool LuaScene::call(const char* fn, int nargs, int nrets) {
    lua_getglobal(lua.L(), fn);
    if (!lua_isfunction(lua.L(), -1)) {
        lua_pop(lua.L(), 1 + nargs);
        return false;
    }
    if (nargs > 0) lua_insert(lua.L(), -1 - nargs);
    if (lua_pcall(lua.L(), nargs, nrets, 0) != LUA_OK) {
        std::cerr << "Lua error in " << fn << ": " << lua_tostring(lua.L(), -1) << "\n";
        lua_pop(lua.L(), 1);
        return false;
    }
    return true;
}

bool LuaScene::ensureHudFont(Engine& engine, const std::string& fontPath, int size) {
    if (hudInited && hudFontPath == fontPath && hudFontSize == size) return true;

    hudText.quit();
    hudInited = hudText.init(engine.getAssets(), fontPath, size);

    if (hudInited) {
        hudFontPath = fontPath;
        hudFontSize = size;
    } else {
        hudFontPath.clear();
        hudFontSize = 0;
    }
    return hudInited;
}

void LuaScene::onEnter(Engine& engine) {
    ctx.engine = &engine;
    ctx.scene = this;

    if (!lua.init()) {
        std::cerr << "LuaVM init failed\n";
        engine.stop();
        return;
    }

    RexLua::install(lua.L(), &ctx);

    if (!lua.runFile(entry)) {
        engine.stop();
        return;
    }

    call("onEnter", 0, 0);
}

void LuaScene::onExit(Engine& engine) {
    (void)engine;
    call("onExit", 0, 0);

    // HUD 텍스트 리소스 정리
    hudText.quit();
    hudInited = false;
    hudFontPath.clear();
    hudFontSize = 0;
}

void LuaScene::update(float dt, Input& input) {
    (void)input;

    lua_pushnumber(lua.L(), dt);
    call("update", 1, 0);

    // 기본 물리 적용 (Lua는 속도만 바꾸고, 이동/충돌은 엔진)
    em.forEachAlive([&](Entity& e) {
        if (e.isCollidable()) ph.apply(e, dt, w);
    });

    em.sweepDead();
}

void LuaScene::render(Graphics& g) {
    (void)g;
    call("render", 0, 0);

    // 필요하면 Lua에서 rex.world.render(), rex.entity.render_all() 호출해서 그림
}
