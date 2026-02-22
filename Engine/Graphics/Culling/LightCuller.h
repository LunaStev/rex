#pragma once

#include "../Lighting/Light.h"

#include <vector>

namespace rex::gfx {

class LightCuller {
public:
    std::vector<RuntimeLight> cullForView(const std::vector<RuntimeLight>& input,
                                          const Vec3& viewPos,
                                          int maxLights) const;
};

} // namespace rex::gfx
