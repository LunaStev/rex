#include "Entity.h"
#include <cmath>
#include "EntityId.h"

static inline void worldToScreenRect(
    const Graphics& gfx,
    float wx, float wy, float ww, float wh,
    int& outX, int& outY, int& outW, int& outH
) {
    const auto& cam = gfx.getCamera();
    const float z = cam.zoom;

    const float sx = (wx - cam.x) * z + (gfx.getViewportW() * 0.5f);
    const float sy = (wy - cam.y) * z + (gfx.getViewportH() * 0.5f);

    outX = (int)std::lround(sx);
    outY = (int)std::lround(sy);
    outW = (int)std::lround(ww * z);
    outH = (int)std::lround(wh * z);
}

void Entity::update(float dt, Input& input, World& world) {
    (void)dt;
    (void)input;
    (void)world;
    // Derived classes only set velocity/state here.
    // Physics::apply() integrates movement/collision.
}

void Entity::render(Graphics& graphics) {
    if (!visible) return;

    if (hasSprite()) {
        graphics.drawSpriteWorld(
            sprite,
            getCenterX(),
            getCenterY(),
            scale,
            rotation,
            flip
        );
        return;
    }

    int sx, sy, sw, sh;
    worldToScreenRect(graphics, x, y, width, height, sx, sy, sw, sh);
    graphics.drawRect(sx, sy, sw, sh, colorR, colorG, colorB, colorA);
}