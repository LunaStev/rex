#include "Entity.h"

void Entity::update(float dt, Input& input, World& world) {
    x += vx * dt;
    y += vy * dt;
}

void Entity::render(Graphics& graphics) {
    graphics.drawRect(
        (int)x, (int)y,
        (int)width, (int)height,
        r, g, b, a
    );
}
