#include "Player.h"
#include "../Input/Input.h"
#include "../World/World.h"

void Player::update(float dt, Input& input, World& world) {
    (void)dt;
    (void)world;

    float dir = 0.0f;

    if (input.isKeyHeld(RexKey::A) || input.isKeyHeld(RexKey::LEFT))  dir -= 1.0f;
    if (input.isKeyHeld(RexKey::D) || input.isKeyHeld(RexKey::RIGHT)) dir += 1.0f;

    setVX(dir * speed);

    if (isOnGround() && input.isKeyPressed(RexKey::SPACE)) {
        setVY(-jump);
        setOnGround(false);
    }
}
