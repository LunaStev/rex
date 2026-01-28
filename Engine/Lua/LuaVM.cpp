#include "LuaVM.h"
#include <iostream>

static std::string dirOf(const std::string& p) {
    auto pos = p.find_last_of("/\\");
    if (pos == std::string::npos) return ".";
    return p.substr(0, pos);
}

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
    // package.path에 스크립트 폴더 추가
    {
        std::string dir = dirOf(path);

        lua_getglobal(state, "package");           // package
        lua_getfield(state, -1, "path");           // package, path
        const char* cur = lua_tostring(state, -1);
        std::string newPath = (dir + "/?.lua;" + dir + "/?/init.lua;") + (cur ? cur : "");
        lua_pop(state, 1);                         // package

        lua_pushstring(state, newPath.c_str());
        lua_setfield(state, -2, "path");           // package.path = newPath
        lua_pop(state, 1);                         // pop package
    }

    if (luaL_dofile(state, path.c_str()) != LUA_OK) {
        std::cerr << "Lua error: " << lua_tostring(state, -1) << "\n";
        lua_pop(state, 1);
        return false;
    }
    return true;
}
