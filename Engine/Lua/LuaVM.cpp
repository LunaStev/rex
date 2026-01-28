#include "LuaVM.h"
#include <iostream>

LuaVM::~LuaVM() {
    if (state) {
        lua_close(state);
        state = nullptr;
    }
}

bool LuaVM::init() {
    state = luaL_newstate();
    if (!state) return false;
    luaL_openlibs(state);
    return true;
}

bool LuaVM::runFile(const std::string& path) {
    if (luaL_dofile(state, path.c_str()) != LUA_OK) {
        std::cerr << "Lua error: " << lua_tostring(state, -1) << "\n";
        lua_pop(state, 1);
        return false;
    }
    return true;
}