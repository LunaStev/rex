#include "Entity.h"

void Entity::update(float dt, Input& input, World& world) {
    // Default: do nothing.
    // Derived classes should change velocity/state here.
    (void)dt;
    (void)input;
    (void)world;

    // IMPORTANT:
    // Do NOT integrate position here. Physics::apply() handles movement/collision.
}

void Entity::render(Graphics& graphics) {
    graphics.drawRect(
        (int)x, (int)y,
        (int)width, (int)height,
        r, g, b, a
    );
}
