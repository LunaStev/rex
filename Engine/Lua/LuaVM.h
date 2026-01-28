#pragma once
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <string>

class LuaVM {
public:
    LuaVM() = default;
    ~LuaVM();

    bool init();
    bool runFile(const std::string& path);

    lua_State* L() { return state; }

private:
    lua_State* state = nullptr;
};