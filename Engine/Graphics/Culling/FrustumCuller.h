#pragma once

#include "../../Core/Components.h"
#include "../../Core/Scene.h"

#include <vector>

namespace rex::gfx {

struct VisibleRenderable {
    EntityId entity = 0;
    Transform* transform = nullptr;
    MeshRenderer* renderer = nullptr;
};

class FrustumCuller {
public:
    std::vector<VisibleRenderable> collectVisible(Scene& scene,
                                                  const Vec3& cameraPos,
                                                  const Vec3& cameraForward,
                                                  float fovDegrees,
                                                  float aspect,
                                                  float nearPlane,
                                                  float farPlane) const;
};

} // namespace rex::gfx
