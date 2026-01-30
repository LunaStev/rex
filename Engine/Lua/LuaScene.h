#pragma once
#include "../Core/Scene.h"
#include "../World/World.h"
#include "../Physics/Physics.h"
#include "../Entity/EntityManager.h"
#include "../Text/Text.h"

#include "LuaVM.h"
#include "LuaBindings.h"

#include <string>

class LuaScene : public Scene {
public:
    explicit LuaScene(std::string entryFile)
        : entry(std::move(entryFile)) {}

    void onEnter(Engine& engine) override;
    void onExit(Engine& engine) override;

    void update(float dt, Input& input) override;
    void render(Graphics& g) override;

    World& world() { return w; }
    EntityManager& entities() { return em; }
    Physics& physics() { return ph; }

    // ---- HUD/Text helper (LuaBindings에서 씀) ----
    bool ensureHudFont(Engine& engine, const std::string& fontPath, int size);
    Text& hud() { return hudText; }

private:
    std::string entry;

    LuaVM lua;
    RexLua::Ctx ctx{nullptr, nullptr};

    World w;
    EntityManager em;
    Physics ph;

    // HUD 텍스트 캐시
    Text hudText;
    bool hudInited = false;
    std::string hudFontPath;
    int hudFontSize = 0;

private:
    bool call(const char* fn, int nargs, int nrets);
};
