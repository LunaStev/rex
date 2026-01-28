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

    // optional hook: onEnter()
    call("onEnter", 0, 0);
}

void LuaScene::onExit(Engine& engine) {
    (void)engine;
    call("onExit", 0, 0);
    // LuaVM dtor가 정리
}

void LuaScene::update(float dt, Input& input) {
    (void)input;

    // update(dt)
    lua_pushnumber(lua.L(), dt);
    call("update", 1, 0);

    // 기본 물리 적용: Lua는 vx/vy만 바꾸고, 실제 이동/충돌은 엔진이 처리
    em.forEachAlive([&](Entity& e) {
        if (e.isCollidable()) ph.apply(e, dt, w);
    });

    em.sweepDead();
}

void LuaScene::render(Graphics& g) {
    // render()
    call("render", 0, 0);

    // 디버그로 월드/엔티티 자동 렌더 보고 싶으면 아래 주석 풀어
    // w.render(g);
    // em.renderAll(g);
}