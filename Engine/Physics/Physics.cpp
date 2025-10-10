#include "Physics.h"

void Physics::apply(Entity& entity, float dt, World& world) {
    // Application of gravity
    entity.setVelocity(entity.getVX(), entity.getVY() + gravity * dt);

    // Update Location
    entity.setPosition(
        entity.getX() + entity.getVX() * dt,
        entity.getY() + entity.getVY() * dt
    );

    // Check the ground collision
    if (checkGroundCollision(entity, world)) {
        entity.setVY(0);
    }
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
