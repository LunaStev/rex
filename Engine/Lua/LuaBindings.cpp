#include "LuaBindings.h"
#include "../Core/Engine.h"
#include "LuaScene.h"
#include "../Input/InputKeys.h"
#include "../Entity/Player.h"

#include <iostream>

namespace {
static constexpr const char* CTX_KEY = "REX_CTX";

static RexLua::Ctx* ctx(lua_State* L) {
    lua_getfield(L, LUA_REGISTRYINDEX, CTX_KEY);
    auto* p = (RexLua::Ctx*)lua_touserdata(L, -1);
    lua_pop(L, 1);
    return p;
}

static int l_log(lua_State* L) {
    const char* s = luaL_checkstring(L, 1);
    std::cout << "[lua] " << s << "\n";
    return 0;
}

// -------- input --------
static int l_input_held(lua_State* L) {
    auto* c = ctx(L);
    int key = (int)luaL_checkinteger(L, 1);
    bool v = c->engine->getInput().isKeyHeld((RexKey)key);
    lua_pushboolean(L, v);
    return 1;
}
static int l_input_pressed(lua_State* L) {
    auto* c = ctx(L);
    int key = (int)luaL_checkinteger(L, 1);
    bool v = c->engine->getInput().isKeyPressed((RexKey)key);
    lua_pushboolean(L, v);
    return 1;
}
static int l_input_released(lua_State* L) {
    auto* c = ctx(L);
    int key = (int)luaL_checkinteger(L, 1);
    bool v = c->engine->getInput().isKeyReleased((RexKey)key);
    lua_pushboolean(L, v);
    return 1;
}

// -------- gfx --------
static int l_gfx_clear(lua_State* L) {
    auto* c = ctx(L);
    int r = (int)luaL_checkinteger(L, 1);
    int g = (int)luaL_checkinteger(L, 2);
    int b = (int)luaL_checkinteger(L, 3);
    int a = (int)luaL_optinteger(L, 4, 255);
    c->engine->getGraphics().clear((Uint8)r,(Uint8)g,(Uint8)b,(Uint8)a);
    return 0;
}
static int l_gfx_rect(lua_State* L) {
    auto* c = ctx(L);
    int x = (int)luaL_checkinteger(L, 1);
    int y = (int)luaL_checkinteger(L, 2);
    int w = (int)luaL_checkinteger(L, 3);
    int h = (int)luaL_checkinteger(L, 4);
    int r = (int)luaL_checkinteger(L, 5);
    int g = (int)luaL_checkinteger(L, 6);
    int b = (int)luaL_checkinteger(L, 7);
    int a = (int)luaL_optinteger(L, 8, 255);
    c->engine->getGraphics().drawRect(x,y,w,h,(Uint8)r,(Uint8)g,(Uint8)b,(Uint8)a);
    return 0;
}

// -------- world --------
static int l_world_generate_flat(lua_State* L) {
    auto* c = ctx(L);
    int w = (int)luaL_checkinteger(L, 1);
    int h = (int)luaL_checkinteger(L, 2);
    int gy = (int)luaL_checkinteger(L, 3);
    c->scene->world().generateFlat(w,h,gy);
    return 0;
}
static int l_world_render(lua_State* L) {
    auto* c = ctx(L);
    c->scene->world().render(c->engine->getGraphics());
    return 0;
}

// -------- entity (EntityId는 Lua 5.4 integer로 64-bit 안전) --------
static lua_Integer packId(EntityId id) {
    return (lua_Integer)(((uint64_t)id.generation << 32) | (uint64_t)id.index);
}
static EntityId unpackId(lua_Integer v) {
    uint64_t u = (uint64_t)v;
    EntityId id;
    id.index = (uint32_t)(u & 0xffffffffu);
    id.generation = (uint32_t)(u >> 32);
    return id;
}

static int l_entity_spawn_player(lua_State* L) {
    auto* c = ctx(L);
    float x = (float)luaL_optnumber(L, 1, 120);
    float y = (float)luaL_optnumber(L, 2, 120);
    EntityId id = c->scene->entities().create<Player>(x, y);
    lua_pushinteger(L, packId(id));
    return 1;
}

static int l_entity_set_velocity(lua_State* L) {
    auto* c = ctx(L);
    EntityId id = unpackId(luaL_checkinteger(L, 1));
    float vx = (float)luaL_checknumber(L, 2);
    float vy = (float)luaL_checknumber(L, 3);

    Entity* e = c->scene->entities().get(id);
    if (!e) return 0;
    e->setVelocity(vx, vy);
    return 0;
}

static int l_entity_get_pos(lua_State* L) {
    auto* c = ctx(L);
    EntityId id = unpackId(luaL_checkinteger(L, 1));
    Entity* e = c->scene->entities().get(id);
    if (!e) { lua_pushnil(L); return 1; }
    lua_pushnumber(L, e->getX());
    lua_pushnumber(L, e->getY());
    return 2;
}

static int l_entity_kill(lua_State* L) {
    auto* c = ctx(L);
    EntityId id = unpackId(luaL_checkinteger(L, 1));
    Entity* e = c->scene->entities().get(id);
    if (e) e->kill();
    return 0;
}

// -------- audio (path 기반) --------
static int l_audio_play_sound(lua_State* L) {
    auto* c = ctx(L);
    const char* path = luaL_checkstring(L, 1);
    int loops = (int)luaL_optinteger(L, 2, 0);

    Mix_Chunk* chunk = c->engine->getAssets().sound(path);
    if (!chunk) { lua_pushboolean(L, 0); return 1; }

    c->engine->getAudio().playSound(chunk, loops);
    lua_pushboolean(L, 1);
    return 1;
}

static void push_keys(lua_State* L) {
    lua_newtable(L); // rex.key

#define K(name) do { lua_pushinteger(L, (lua_Integer)RexKey::name); lua_setfield(L, -2, #name); } while(0)
    K(A); K(D); K(LEFT); K(RIGHT); K(SPACE); K(ESCAPE);
#undef K
}

static int luaopen_rex(lua_State* L) {
    lua_newtable(L); // rex

    // rex.log
    lua_pushcfunction(L, l_log);
    lua_setfield(L, -2, "log");

    // rex.key
    push_keys(L);
    lua_setfield(L, -2, "key");

    // rex.input
    lua_newtable(L);
    lua_pushcfunction(L, l_input_held);    lua_setfield(L, -2, "held");
    lua_pushcfunction(L, l_input_pressed); lua_setfield(L, -2, "pressed");
    lua_pushcfunction(L, l_input_released);lua_setfield(L, -2, "released");
    lua_setfield(L, -2, "input");

    // rex.gfx
    lua_newtable(L);
    lua_pushcfunction(L, l_gfx_clear); lua_setfield(L, -2, "clear");
    lua_pushcfunction(L, l_gfx_rect);  lua_setfield(L, -2, "rect");
    lua_setfield(L, -2, "gfx");

    // rex.world
    lua_newtable(L);
    lua_pushcfunction(L, l_world_generate_flat); lua_setfield(L, -2, "generate_flat");
    lua_pushcfunction(L, l_world_render);        lua_setfield(L, -2, "render");
    lua_setfield(L, -2, "world");

    // rex.entity
    lua_newtable(L);
    lua_pushcfunction(L, l_entity_spawn_player); lua_setfield(L, -2, "spawn_player");
    lua_pushcfunction(L, l_entity_set_velocity); lua_setfield(L, -2, "set_velocity");
    lua_pushcfunction(L, l_entity_get_pos);      lua_setfield(L, -2, "get_pos");
    lua_pushcfunction(L, l_entity_kill);         lua_setfield(L, -2, "kill");
    lua_setfield(L, -2, "entity");

    // rex.audio
    lua_newtable(L);
    lua_pushcfunction(L, l_audio_play_sound); lua_setfield(L, -2, "play_sound");
    lua_setfield(L, -2, "audio");

    return 1; // return rex table
}
} // namespace

namespace RexLua {
void install(lua_State* L, Ctx* c) {
    // registry에 ctx 저장
    lua_pushlightuserdata(L, c);
    lua_setfield(L, LUA_REGISTRYINDEX, CTX_KEY);

    // require("rex") 등록
    luaL_requiref(L, "rex", luaopen_rex, 1);
    lua_pop(L, 1);
}
}