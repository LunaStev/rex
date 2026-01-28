#include "LuaBindings.h"
#include "../Core/Engine.h"
#include "LuaScene.h"

#include "../Input/InputKeys.h"
#include "../Entity/Player.h"
#include "../Entity/Entity.h"
#include "../Graphics/Texture.h" // Texture::handle(), width(), height() 필요

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <iostream>
#include <string>
#include <cstdint>
#include <algorithm>

namespace {

static constexpr const char* CTX_KEY = "REX_CTX";

static RexLua::Ctx* ctx(lua_State* L) {
    lua_getfield(L, LUA_REGISTRYINDEX, CTX_KEY);
    auto* p = (RexLua::Ctx*)lua_touserdata(L, -1);
    lua_pop(L, 1);
    return p;
}

static RexLua::Ctx* must_ctx(lua_State* L) {
    auto* c = ctx(L);
    if (!c || !c->engine || !c->scene) {
        luaL_error(L, "rex context missing");
        return nullptr;
    }
    return c;
}

static int l_log(lua_State* L) {
    const char* s = luaL_checkstring(L, 1);
    std::cout << "[lua] " << s << "\n";
    return 0;
}

// ---------------- input ----------------
static int l_input_held(lua_State* L) {
    auto* c = must_ctx(L);
    int key = (int)luaL_checkinteger(L, 1);
    lua_pushboolean(L, c->engine->getInput().isKeyHeld((RexKey)key));
    return 1;
}
static int l_input_pressed(lua_State* L) {
    auto* c = must_ctx(L);
    int key = (int)luaL_checkinteger(L, 1);
    lua_pushboolean(L, c->engine->getInput().isKeyPressed((RexKey)key));
    return 1;
}
static int l_input_released(lua_State* L) {
    auto* c = must_ctx(L);
    int key = (int)luaL_checkinteger(L, 1);
    lua_pushboolean(L, c->engine->getInput().isKeyReleased((RexKey)key));
    return 1;
}

// ---------------- gfx ----------------
static int l_gfx_clear(lua_State* L) {
    auto* c = must_ctx(L);
    int r = (int)luaL_checkinteger(L, 1);
    int g = (int)luaL_checkinteger(L, 2);
    int b = (int)luaL_checkinteger(L, 3);
    int a = (int)luaL_optinteger(L, 4, 255);
    c->engine->getGraphics().clear((Uint8)r,(Uint8)g,(Uint8)b,(Uint8)a);
    return 0;
}
static int l_gfx_rect(lua_State* L) {
    auto* c = must_ctx(L);
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

// rex.gfx.image(nameOrPath, x, y, [w], [h]) -> bool
static int l_gfx_image(lua_State* L) {
    auto* c = must_ctx(L);
    const char* nameOrPath = luaL_checkstring(L, 1);
    int x = (int)luaL_checkinteger(L, 2);
    int y = (int)luaL_checkinteger(L, 3);
    int w = (int)luaL_optinteger(L, 4, 0);
    int h = (int)luaL_optinteger(L, 5, 0);

    Texture* t = c->engine->getAssets().texture(nameOrPath);
    if (!t || !t->handle()) { lua_pushboolean(L, 0); return 1; }

    if (w <= 0) w = t->width();
    if (h <= 0) h = t->height();

    SDL_Rect dst{ x, y, w, h };
    SDL_RenderCopy(c->engine->getSDLRenderer(), t->handle(), nullptr, &dst);

    lua_pushboolean(L, 1);
    return 1;
}

// ---------------- world ----------------
static int l_world_generate_flat(lua_State* L) {
    auto* c = must_ctx(L);
    int w = (int)luaL_checkinteger(L, 1);
    int h = (int)luaL_checkinteger(L, 2);
    int gy = (int)luaL_checkinteger(L, 3);
    c->scene->world().generateFlat(w,h,gy);
    return 0;
}
static int l_world_render(lua_State* L) {
    auto* c = must_ctx(L);
    c->scene->world().render(c->engine->getGraphics());
    return 0;
}
static int l_world_set_tile(lua_State* L) {
    auto* c = must_ctx(L);
    int x = (int)luaL_checkinteger(L, 1);
    int y = (int)luaL_checkinteger(L, 2);
    int t = (int)luaL_checkinteger(L, 3);
    c->scene->world().setTile(x, y, t);
    return 0;
}
static int l_world_get_tile(lua_State* L) {
    auto* c = must_ctx(L);
    int x = (int)luaL_checkinteger(L, 1);
    int y = (int)luaL_checkinteger(L, 2);
    int t = c->scene->world().getTile(x, y);
    lua_pushinteger(L, t);
    return 1;
}
static int l_world_size(lua_State* L) {
    auto* c = must_ctx(L);
    lua_pushinteger(L, c->scene->world().width);
    lua_pushinteger(L, c->scene->world().height);
    return 2;
}
static int l_world_tile_size(lua_State* L) {
    (void)L;
    lua_pushinteger(L, (lua_Integer)World::TILE_SIZE);
    return 1;
}

// ---------------- entity id pack ----------------
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
static Entity* get_entity(lua_State* L, RexLua::Ctx* c, int idx) {
    EntityId id = unpackId(luaL_checkinteger(L, idx));
    return c->scene->entities().get(id);
}

// ---------------- entity ----------------
static int l_entity_spawn_player(lua_State* L) {
    auto* c = must_ctx(L);
    float x = (float)luaL_optnumber(L, 1, 120);
    float y = (float)luaL_optnumber(L, 2, 120);
    EntityId id = c->scene->entities().create<Player>(x, y);
    lua_pushinteger(L, packId(id));
    return 1;
}

// rex.entity.spawn_box([x],[y],[w],[h]) -> id
static int l_entity_spawn_box(lua_State* L) {
    auto* c = must_ctx(L);
    float x = (float)luaL_optnumber(L, 1, 100);
    float y = (float)luaL_optnumber(L, 2, 100);
    float w = (float)luaL_optnumber(L, 3, 32);
    float h = (float)luaL_optnumber(L, 4, 32);
    EntityId id = c->scene->entities().create<Entity>(x, y, w, h);
    lua_pushinteger(L, packId(id));
    return 1;
}

static int l_entity_set_velocity(lua_State* L) {
    auto* c = must_ctx(L);
    Entity* e = get_entity(L, c, 1);
    if (!e) return 0;
    float vx = (float)luaL_checknumber(L, 2);
    float vy = (float)luaL_checknumber(L, 3);
    e->setVelocity(vx, vy);
    return 0;
}

static int l_entity_set_pos(lua_State* L) {
    auto* c = must_ctx(L);
    Entity* e = get_entity(L, c, 1);
    if (!e) return 0;
    float x = (float)luaL_checknumber(L, 2);
    float y = (float)luaL_checknumber(L, 3);
    e->setPosition(x, y);
    return 0;
}

static int l_entity_get_pos(lua_State* L) {
    auto* c = must_ctx(L);
    Entity* e = get_entity(L, c, 1);
    if (!e) { lua_pushnil(L); return 1; }
    lua_pushnumber(L, e->getX());
    lua_pushnumber(L, e->getY());
    return 2;
}

static int l_entity_set_color(lua_State* L) {
    auto* c = must_ctx(L);
    Entity* e = get_entity(L, c, 1);
    if (!e) return 0;
    int r = (int)luaL_checkinteger(L, 2);
    int g = (int)luaL_checkinteger(L, 3);
    int b = (int)luaL_checkinteger(L, 4);
    int a = (int)luaL_optinteger(L, 5, 255);
    e->setColor((Uint8)r,(Uint8)g,(Uint8)b,(Uint8)a);
    return 0;
}

static int l_entity_is_alive(lua_State* L) {
    auto* c = must_ctx(L);
    Entity* e = get_entity(L, c, 1);
    lua_pushboolean(L, e && e->isAlive());
    return 1;
}

static int l_entity_kill(lua_State* L) {
    auto* c = must_ctx(L);
    Entity* e = get_entity(L, c, 1);
    if (e) e->kill();
    return 0;
}

static int l_entity_render(lua_State* L) {
    auto* c = must_ctx(L);
    Entity* e = get_entity(L, c, 1);
    if (!e) return 0;
    e->render(c->engine->getGraphics());
    return 0;
}

static int l_entity_render_all(lua_State* L) {
    auto* c = must_ctx(L);
    c->scene->entities().renderAll(c->engine->getGraphics());
    return 0;
}

// ---------------- assets ----------------
static int l_assets_add_texture(lua_State* L) {
    auto* c = must_ctx(L);
    const char* name = luaL_checkstring(L, 1);
    const char* path = luaL_checkstring(L, 2);
    lua_pushboolean(L, c->engine->getAssets().addTexture(name, path));
    return 1;
}
static int l_assets_add_sound(lua_State* L) {
    auto* c = must_ctx(L);
    const char* name = luaL_checkstring(L, 1);
    const char* path = luaL_checkstring(L, 2);
    lua_pushboolean(L, c->engine->getAssets().addSound(name, path));
    return 1;
}
static int l_assets_add_music(lua_State* L) {
    auto* c = must_ctx(L);
    const char* name = luaL_checkstring(L, 1);
    const char* path = luaL_checkstring(L, 2);
    lua_pushboolean(L, c->engine->getAssets().addMusic(name, path));
    return 1;
}
static int l_assets_add_font(lua_State* L) {
    auto* c = must_ctx(L);
    const char* name = luaL_checkstring(L, 1);
    const char* path = luaL_checkstring(L, 2);
    int size = (int)luaL_checkinteger(L, 3);
    lua_pushboolean(L, c->engine->getAssets().addFont(name, path, size));
    return 1;
}

// ---------------- audio ----------------
// play_sound(nameOrPath[, loops]) -> bool
static int l_audio_play_sound(lua_State* L) {
    auto* c = must_ctx(L);
    const char* nameOrPath = luaL_checkstring(L, 1);
    int loops = (int)luaL_optinteger(L, 2, 0);

    Mix_Chunk* chunk = c->engine->getAssets().sound(nameOrPath);
    if (!chunk) { lua_pushboolean(L, 0); return 1; }

    c->engine->getAudio().playSound(chunk, loops);
    lua_pushboolean(L, 1);
    return 1;
}

// play_music(nameOrPath[, loops]) -> bool
static int l_audio_play_music(lua_State* L) {
    auto* c = must_ctx(L);
    const char* nameOrPath = luaL_checkstring(L, 1);
    int loops = (int)luaL_optinteger(L, 2, -1);

    Mix_Music* music = c->engine->getAssets().music(nameOrPath);
    if (!music) { lua_pushboolean(L, 0); return 1; }

    bool ok = c->engine->getAudio().playMusic(music, loops);
    lua_pushboolean(L, ok);
    return 1;
}

static int l_audio_stop_music(lua_State* L)  { (void)L; Mix_HaltMusic(); return 0; }
static int l_audio_pause_music(lua_State* L) { (void)L; Mix_PauseMusic(); return 0; }
static int l_audio_resume_music(lua_State* L){ (void)L; Mix_ResumeMusic(); return 0; }
static int l_audio_volume_music(lua_State* L){
    int v = (int)luaL_checkinteger(L, 1); // 0~128
    v = std::clamp(v, 0, 128);
    Mix_VolumeMusic(v);
    return 0;
}

// ---------------- text ----------------
// rex.text.draw(fontPath, size, text, x, y, [r,g,b,a]) -> bool
static int l_text_draw(lua_State* L) {
    auto* c = must_ctx(L);
    const char* fontPath = luaL_checkstring(L, 1);
    int size = (int)luaL_checkinteger(L, 2);
    const char* msg = luaL_checkstring(L, 3);
    int x = (int)luaL_checkinteger(L, 4);
    int y = (int)luaL_checkinteger(L, 5);
    int r = (int)luaL_optinteger(L, 6, 255);
    int g = (int)luaL_optinteger(L, 7, 255);
    int b = (int)luaL_optinteger(L, 8, 255);
    int a = (int)luaL_optinteger(L, 9, 255);

    if (!c->scene->ensureHudFont(*c->engine, fontPath, size)) {
        lua_pushboolean(L, 0);
        return 1;
    }

    SDL_Color col{ (Uint8)r,(Uint8)g,(Uint8)b,(Uint8)a };
    c->scene->hud().render(c->engine->getGraphics(), msg, x, y, col);

    lua_pushboolean(L, 1);
    return 1;
}

// ---------------- scene control ----------------
static int l_scene_replace(lua_State* L) {
    auto* c = must_ctx(L);
    const char* entry = luaL_checkstring(L, 1);
    c->engine->scenes.replace(std::make_unique<LuaScene>(entry));
    return 0;
}
static int l_scene_push(lua_State* L) {
    auto* c = must_ctx(L);
    const char* entry = luaL_checkstring(L, 1);
    c->engine->scenes.push(std::make_unique<LuaScene>(entry));
    return 0;
}
static int l_scene_pop(lua_State* L) {
    auto* c = must_ctx(L);
    c->engine->scenes.pop();
    return 0;
}

// ---------------- keys table ----------------
static void push_keys(lua_State* L) {
    lua_newtable(L); // rex.key

#define K(name) do { lua_pushinteger(L, (lua_Integer)RexKey::name); lua_setfield(L, -2, #name); } while(0)
    // movement
    K(W); K(A); K(S); K(D);
    K(UP); K(DOWN); K(LEFT); K(RIGHT);

    // actions
    K(SPACE); K(RETURN); K(ESCAPE); K(I); K(M); K(R);

    // digits
    K(N1); K(N2); K(N3); K(N4); K(N5); K(N6); K(N7); K(N8); K(N9);
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
    lua_pushcfunction(L, l_gfx_image); lua_setfield(L, -2, "image");
    lua_setfield(L, -2, "gfx");

    // rex.world
    lua_newtable(L);
    lua_pushcfunction(L, l_world_generate_flat); lua_setfield(L, -2, "generate_flat");
    lua_pushcfunction(L, l_world_set_tile);      lua_setfield(L, -2, "set_tile");
    lua_pushcfunction(L, l_world_get_tile);      lua_setfield(L, -2, "get_tile");
    lua_pushcfunction(L, l_world_size);          lua_setfield(L, -2, "size");
    lua_pushcfunction(L, l_world_tile_size);     lua_setfield(L, -2, "tile_size");
    lua_pushcfunction(L, l_world_render);        lua_setfield(L, -2, "render");
    lua_setfield(L, -2, "world");

    // rex.entity
    lua_newtable(L);
    lua_pushcfunction(L, l_entity_spawn_player); lua_setfield(L, -2, "spawn_player");
    lua_pushcfunction(L, l_entity_spawn_box);    lua_setfield(L, -2, "spawn_box");
    lua_pushcfunction(L, l_entity_set_pos);      lua_setfield(L, -2, "set_pos");
    lua_pushcfunction(L, l_entity_get_pos);      lua_setfield(L, -2, "get_pos");
    lua_pushcfunction(L, l_entity_set_velocity); lua_setfield(L, -2, "set_velocity");
    lua_pushcfunction(L, l_entity_set_color);    lua_setfield(L, -2, "set_color");
    lua_pushcfunction(L, l_entity_is_alive);     lua_setfield(L, -2, "is_alive");
    lua_pushcfunction(L, l_entity_kill);         lua_setfield(L, -2, "kill");
    lua_pushcfunction(L, l_entity_render);       lua_setfield(L, -2, "render");
    lua_pushcfunction(L, l_entity_render_all);   lua_setfield(L, -2, "render_all");
    lua_setfield(L, -2, "entity");

    // rex.assets
    lua_newtable(L);
    lua_pushcfunction(L, l_assets_add_texture); lua_setfield(L, -2, "add_texture");
    lua_pushcfunction(L, l_assets_add_sound);   lua_setfield(L, -2, "add_sound");
    lua_pushcfunction(L, l_assets_add_music);   lua_setfield(L, -2, "add_music");
    lua_pushcfunction(L, l_assets_add_font);    lua_setfield(L, -2, "add_font");
    lua_setfield(L, -2, "assets");

    // rex.audio
    lua_newtable(L);
    lua_pushcfunction(L, l_audio_play_sound);   lua_setfield(L, -2, "play_sound");
    lua_pushcfunction(L, l_audio_play_music);   lua_setfield(L, -2, "play_music");
    lua_pushcfunction(L, l_audio_stop_music);   lua_setfield(L, -2, "stop_music");
    lua_pushcfunction(L, l_audio_pause_music);  lua_setfield(L, -2, "pause_music");
    lua_pushcfunction(L, l_audio_resume_music); lua_setfield(L, -2, "resume_music");
    lua_pushcfunction(L, l_audio_volume_music); lua_setfield(L, -2, "volume_music");
    lua_setfield(L, -2, "audio");

    // rex.text
    lua_newtable(L);
    lua_pushcfunction(L, l_text_draw); lua_setfield(L, -2, "draw");
    lua_setfield(L, -2, "text");

    // rex.scene
    lua_newtable(L);
    lua_pushcfunction(L, l_scene_replace); lua_setfield(L, -2, "replace");
    lua_pushcfunction(L, l_scene_push);    lua_setfield(L, -2, "push");
    lua_pushcfunction(L, l_scene_pop);     lua_setfield(L, -2, "pop");
    lua_setfield(L, -2, "scene");

    return 1; // return rex table
}

} // namespace

namespace RexLua {
void install(lua_State* L, Ctx* c) {
    lua_pushlightuserdata(L, c);
    lua_setfield(L, LUA_REGISTRYINDEX, CTX_KEY);

    luaL_requiref(L, "rex", luaopen_rex, 1);
    lua_pop(L, 1);
}
} // namespace RexLua
