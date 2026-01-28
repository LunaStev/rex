#include "Physics.h"
#include "../Entity/Entity.h"
#include "../World/World.h"

#include <algorithm>
#include <cmath>

static inline int worldToTile(float v, float TS) {
    // 음수 좌표도 안전하게 처리
    return (int)std::floor(v / TS);
}

void Physics::apply(Entity& entity, float dt, World& world) {
    if (dt <= 0.0f) return;

    // 디버거 멈춤/랙 등으로 dt 폭발하는 거 제한
    dt = std::clamp(dt, 0.0f, 0.25f);

    const float TS = (float)World::TILE_SIZE;
    const float eps = skin;

    // 월드 밖 접근 방지 + 정책(밖은 벽처럼 처리)
    auto solidSafe = [&](int tx, int ty) -> bool {
        if (tx < 0 || ty < 0 || tx >= world.width || ty >= world.height) return true;
        return world.isSolid(tx, ty);
    };

    bool onGround = false;

    float x = entity.getX();
    float y = entity.getY();
    float w = entity.getWidth();
    float h = entity.getHeight();

    float vx = entity.getVX();
    float vy = entity.getVY();

    // collidable=false면 충돌 없이 움직이기만
    if (!entity.isCollidable()) {
        vy = std::min(vy + gravity * dt, terminalVelocity);
        x += vx * dt;
        y += vy * dt;

        float maxX = world.width  * TS - w;
        float maxY = world.height * TS - h;

        if (x < 0)   { x = 0;    vx = 0; }
        if (x > maxX){ x = maxX; vx = 0; }
        if (y < 0)   { y = 0;    vy = 0; }
        if (y > maxY){ y = maxY; vy = 0; onGround = true; }

        entity.setPosition(x, y);
        entity.setVelocity(vx, vy);
        entity.setOnGround(onGround);
        return;
    }

    // ---------
    // Sub-stepping
    // ---------
    float remaining = dt;
    for (int stepCount = 0; stepCount < maxSubSteps && remaining > 0.0f; ++stepCount) {
        float step = std::min(remaining, maxStep);
        remaining -= step;

        // gravity
        vy = std::min(vy + gravity * step, terminalVelocity);

        // --------------------
        // X axis move + collide
        // --------------------
        float newX = x + vx * step;

        if (vx > 0.0f) { // right
            int tileX = worldToTile(newX + w - eps, TS);
            int top    = worldToTile(y + eps, TS);
            int bottom = worldToTile(y + h - eps, TS);

            for (int ty = top; ty <= bottom; ++ty) {
                if (solidSafe(tileX, ty)) {
                    newX = tileX * TS - w;
                    vx = 0.0f;
                    break;
                }
            }
        } else if (vx < 0.0f) { // left
            int tileX = worldToTile(newX + eps, TS);
            int top    = worldToTile(y + eps, TS);
            int bottom = worldToTile(y + h - eps, TS);

            for (int ty = top; ty <= bottom; ++ty) {
                if (solidSafe(tileX, ty)) {
                    newX = (tileX + 1) * TS;
                    vx = 0.0f;
                    break;
                }
            }
        }

        x = newX;

        // --------------------
        // Y axis move + collide
        // --------------------
        float newY = y + vy * step;

        if (vy > 0.0f) { // down
            int tileY = worldToTile(newY + h - eps, TS);
            int left  = worldToTile(x + eps, TS);
            int right = worldToTile(x + w - eps, TS);

            for (int tx = left; tx <= right; ++tx) {
                if (solidSafe(tx, tileY)) {
                    newY = tileY * TS - h;
                    vy = 0.0f;
                    onGround = true;
                    break;
                }
            }
        } else if (vy < 0.0f) { // up
            int tileY = worldToTile(newY + eps, TS);
            int left  = worldToTile(x + eps, TS);
            int right = worldToTile(x + w - eps, TS);

            for (int tx = left; tx <= right; ++tx) {
                if (solidSafe(tx, tileY)) {
                    newY = (tileY + 1) * TS;
                    vy = 0.0f;
                    break;
                }
            }
        }

        y = newY;
    }

    // 월드 경계 clamp
    float maxX = world.width  * TS - w;
    float maxY = world.height * TS - h;

    if (x < 0)   { x = 0;    vx = 0; }
    if (x > maxX){ x = maxX; vx = 0; }
    if (y < 0)   { y = 0;    vy = 0; }
    if (y > maxY){ y = maxY; vy = 0; onGround = true; }

    entity.setPosition(x, y);
    entity.setVelocity(vx, vy);
    entity.setOnGround(onGround);
}

bool Physics::checkGroundCollision(Entity& entity, World& world) {
    const float TS = (float)World::TILE_SIZE;
    const float eps = skin;

    auto solidSafe = [&](int tx, int ty) -> bool {
        if (tx < 0 || ty < 0 || tx >= world.width || ty >= world.height) return true;
        return world.isSolid(tx, ty);
    };

    float x = entity.getX();
    float y = entity.getY();
    float w = entity.getWidth();
    float h = entity.getHeight();

    // 발밑 한 칸(혹은 eps) 아래를 확인
    int tileY = worldToTile(y + h + eps, TS);
    int left  = worldToTile(x + eps, TS);
    int right = worldToTile(x + w - eps, TS);

    for (int tx = left; tx <= right; ++tx) {
        if (solidSafe(tx, tileY)) {
            // 바닥 위로 올려서 고정
            float groundTop = tileY * TS;
            entity.setPosition(x, groundTop - h);
            entity.setVelocity(entity.getVX(), 0.0f);
            entity.setOnGround(true);
            return true;
        }
    }

    return false;
}

bool Physics::isColliding(const Entity& a, const Entity& b) {
    return (
        a.getX() < b.getX() + b.getWidth() &&
        a.getX() + a.getWidth() > b.getX() &&
        a.getY() < b.getY() + b.getHeight() &&
        a.getY() + a.getHeight() > b.getY()
    );
}
