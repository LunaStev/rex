#include "Physics.h"
#include <algorithm>

void Physics::apply(Entity& entity, float dt, World& world) {
    dt = std::min(dt, 0.05f);

    const float TS = (float)World::TILE_SIZE;

    entity.setOnGround(false);

    float x = entity.getX();
    float y = entity.getY();
    float w = entity.getWidth();
    float h = entity.getHeight();

    float vx = entity.getVX();
    float vy = entity.getVY();

    // gravity
    vy += gravity * dt;

    // --------------------
    // X axis move + collide
    // --------------------
    float newX = x + vx * dt;

    if (vx > 0.0f) { // moving right
        int tileX = (int)((newX + w - 1) / TS);
        int top    = (int)(y / TS);
        int bottom = (int)((y + h - 1) / TS);

        for (int ty = top; ty <= bottom; ++ty) {
            if (world.isSolid(tileX, ty)) {
                newX = tileX * TS - w;
                vx = 0.0f;
                break;
            }
        }
    } else if (vx < 0.0f) { // moving left
        int tileX = (int)(newX / TS);
        int top    = (int)(y / TS);
        int bottom = (int)((y + h - 1) / TS);

        for (int ty = top; ty <= bottom; ++ty) {
            if (world.isSolid(tileX, ty)) {
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
    float newY = y + vy * dt;

    if (vy > 0.0f) { // falling (down)
        int tileY = (int)((newY + h - 1) / TS);
        int left  = (int)(x / TS);
        int right = (int)((x + w - 1) / TS);

        for (int tx = left; tx <= right; ++tx) {
            if (world.isSolid(tx, tileY)) {
                newY = tileY * TS - h;
                vy = 0.0f;
                entity.setOnGround(true);
                break;
            }
        }
    } else if (vy < 0.0f) { // jumping (up)
        int tileY = (int)(newY / TS);
        int left  = (int)(x / TS);
        int right = (int)((x + w - 1) / TS);

        for (int tx = left; tx <= right; ++tx) {
            if (world.isSolid(tx, tileY)) {
                newY = (tileY + 1) * TS;
                vy = 0.0f;
                break;
            }
        }
    }

    y = newY;

    float maxX = world.width  * TS - w;
    float maxY = world.height * TS - h;

    if (x < 0)   { x = 0;   vx = 0; }
    if (x > maxX){ x = maxX;vx = 0; }
    if (y < 0)   { y = 0;   vy = 0; }
    if (y > maxY){ y = maxY;vy = 0; entity.setOnGround(true); }

    entity.setPosition(x, y);
    entity.setVelocity(vx, vy);
}

bool Physics::checkGroundCollision(Entity& entity, World& world) {
    float bottom = entity.getY() + entity.getHeight();
    float groundY = world.getGroundY(entity.getX() + entity.getWidth() / 2, bottom);

    if (bottom >= groundY) {
        entity.setPosition(entity.getX(), groundY - entity.getHeight());
        return true;
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
