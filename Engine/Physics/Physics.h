#pragma once
#include <cstdint>

class Entity;
class World;

class Physics {
public:
    float gravity = 900.0f;

    // 너무 큰 dt에서도 튕기지 않게
    float terminalVelocity = 4000.0f;

    // 서브스텝 (기본 120Hz)
    float maxStep = 1.0f / 120.0f;
    int   maxSubSteps = 8;

    // 타일 경계 붙을 때 끈적임 방지용 epsilon
    float skin = 0.001f;

    // 기본 중력/이동/타일 충돌
    void apply(Entity& entity, float dt, World& world);

    // 바닥에 붙었는지 빠르게 확인(타일 기반)
    bool checkGroundCollision(Entity& entity, World& world);

    // AABB 겹침 판정
    static bool isColliding(const Entity& a, const Entity& b);
};
