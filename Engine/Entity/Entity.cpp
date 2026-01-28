#include "Entity.h"
#include "EntityId.h"

void Entity::_setId(EntityId id) {
    _idValue = id;
}

EntityId Entity::getId() const {
    return _idValue;
}

void Entity::update(float dt, Input& input, World& world) {
    (void)dt;
    (void)input;
    (void)world;
    // Derived classes only set velocity/state here.
    // Physics::apply() integrates movement/collision.
}

static inline void worldToScreenRect(
    const Graphics& g,
    float wx, float wy, float ww, float wh,
    int& outX, int& outY, int& outW, int& outH
) {
    const auto& cam = g.getCamera();
    const float z = cam.zoom;

    const float sx = (wx - cam.x) * z + (g.getViewportW() * 0.5f);
    const float sy = (wy - cam.y) * z + (g.getViewportH() * 0.5f);

    outX = (int)std::lround(sx);
    outY = (int)std::lround(sy);
    outW = (int)std::lround(ww * z);
    outH = (int)std::lround(wh * z);
}

void Entity::render(Graphics& graphics) {
    if (!visible) return;

    if (hasSprite()) {
        // physics는 top-left 기반이라, 렌더는 center 기준으로 맞춰주는 게 편함
        g.drawSpriteWorld(
            sprite,
            getCenterX(),
            getCenterY(),
            scale,
            rotation,
            flip
        );
        return;
    }

    // debug rect (world -> screen with camera)
    int sx, sy, sw, sh;
    worldToScreenRect(graphics, x, y, width, height, sx, sy, sw, sh);
    graphics.drawRect(sx, sy, sw, sh, colorR, colorG, colorB, colorA);
}
