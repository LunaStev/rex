#pragma once
#include "../Entity/Entity.h"
#include "../World/World.h"

class Physics {
public:
    float gravity = 900.0f;

    // Basic gravity/movement/collision handling
    void apply(Entity& entity, float dt, World& world);

    // Determination of collision
    bool checkGroundCollision(Entity& entity, World& world);

    // Simple Crash Judgment (AABB)
    static bool isColliding(const Entity& a, const Entity& b);
};
