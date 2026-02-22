#include "FrustumCuller.h"

#include <algorithm>
#include <cmath>

namespace rex::gfx {

namespace {

constexpr float DEG2RAD = 0.01745329251994329577f;

float max3(float a, float b, float c) {
    return std::max(a, std::max(b, c));
}

Vec3 normalizeSafe(const Vec3& v) {
    const float lenSq = dot(v, v);
    if (lenSq <= 1e-8f) return {0, 0, 1};
    const float invLen = 1.0f / std::sqrt(lenSq);
    return {v.x * invLen, v.y * invLen, v.z * invLen};
}

} // namespace

std::vector<VisibleRenderable> FrustumCuller::collectVisible(Scene& scene,
                                                             const Vec3& cameraPos,
                                                             const Vec3& cameraForward,
                                                             float fovDegrees,
                                                             float aspect,
                                                             float nearPlane,
                                                             float farPlane) const {
    std::vector<VisibleRenderable> visible;

    const Vec3 forward = normalizeSafe(cameraForward);
    const float halfFov = std::max(1.0f, fovDegrees) * DEG2RAD * 0.5f;
    const float tanHalfFov = std::tan(halfFov);
    const float tanHalfFovH = tanHalfFov * std::max(0.1f, aspect);

    scene.each<MeshRenderer>([&](EntityId id, MeshRenderer& renderer) {
        auto* transform = scene.getComponent<Transform>(id);
        if (!transform) return;

        const float radius = max3(std::fabs(transform->scale.x),
                                  std::fabs(transform->scale.y),
                                  std::fabs(transform->scale.z)) * 0.9f + 0.15f;

        const Vec3 toObj = transform->position - cameraPos;
        const float depth = dot(toObj, forward);

        if (depth < nearPlane - radius) return;
        if (depth > farPlane + radius) return;

        const float distSq = dot(toObj, toObj);
        const float lateralSq = std::max(0.0f, distSq - depth * depth);
        const float lateral = std::sqrt(lateralSq);

        const float limitY = depth * tanHalfFov + radius;
        const float limitX = depth * tanHalfFovH + radius;

        if (lateral > std::max(limitX, limitY)) {
            return;
        }

        visible.push_back(VisibleRenderable{id, transform, &renderer});
    });

    return visible;
}

} // namespace rex::gfx
