#pragma once
#include "../Graphics/Graphics.h"

class Game {
public:
    virtual ~Game() = default;

    virtual void update(float dt, Input& input) = 0;
    virtual void render(Graphics& g) = 0;
};