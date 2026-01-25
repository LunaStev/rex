#pragma once
#include "../Engine/Core/Scene.h"
#include "../Engine/Core/Engine.h"

class MainScene : public Scene {
public:
    void onEnter(Engine& engine) override {
        (void)engine;
        // init stuff
    }

    void update(float dt, Input& input) override {
        (void)dt;
        if (input.isKeyPressed(RexKey::SPACE)) {
            // do something
        }
    }

    void render(Graphics& g) override {
        g.drawRect(100, 100, 80, 80, 255, 120, 60, 255);
    }
};
