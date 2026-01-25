#pragma once
#include "Entity.h"

class Player : public Entity {
public:
    float speed = 300.0f;
    float jump  = 520.0f;

    Player(float x=120, float y=120)
        : Entity(x, y, 30, 38) {
        setColor(255, 140, 0, 255);
    }

    void update(float dt, Input& input, World& world) override;
};
