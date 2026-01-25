#pragma once
#include <utility>
#include <type_traits>
#include "Scene.h"
#include "Game.h"

template<typename TGame>
class GameSceneAdapter final : public Scene {
    static_assert(std::is_base_of_v<Game, TGame>, "TGame must derive from Game");

public:
    GameSceneAdapter() = default;

    template<typename... Args>
    explicit GameSceneAdapter(Args&&... args)
        : game(std::forward<Args>(args)...) {}

    void update(float dt, Input& input) override {
        game.update(dt, input);
    }

    void render(Graphics& g) override {
        game.render(g);
    }

private:
    TGame game;
};
