#pragma once

#include "../../Core/Components.h"

namespace rex::gfx {

enum class LightKind : int {
    Directional = 0,
    Point = 1,
    Spot = 2,
    Area = 3,
};

struct RuntimeLight {
    LightKind kind = LightKind::Point;
    Vec3 position{0.0f, 0.0f, 0.0f};
    Vec3 direction{0.0f, -1.0f, 0.0f};
    Vec3 color{1.0f, 1.0f, 1.0f};
    float intensity = 1.0f;

    float range = 20.0f;
    float innerConeCos = 0.9f;
    float outerConeCos = 0.8f;

    Vec3 attenuation{1.0f, 0.09f, 0.032f};

    bool castShadows = false;
    bool volumetric = false;
};

} // namespace rex::gfx
