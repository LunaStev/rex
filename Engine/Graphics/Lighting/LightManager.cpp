#include "LightManager.h"

#include <algorithm>
#include <cmath>

namespace rex::gfx {

namespace {

constexpr float DEG2RAD = 0.01745329251994329577f;

Vec3 normalizeSafe(const Vec3& v) {
    const float lenSq = dot(v, v);
    if (lenSq <= 1e-8f) return {0.0f, -1.0f, 0.0f};
    const float invLen = 1.0f / std::sqrt(lenSq);
    return {v.x * invLen, v.y * invLen, v.z * invLen};
}

Vec3 directionFromEulerDeg(const Vec3& eulerDeg) {
    const float yaw = eulerDeg.y * DEG2RAD;
    const float pitch = eulerDeg.x * DEG2RAD;
    const float cy = std::cos(yaw);
    const float sy = std::sin(yaw);
    const float cp = std::cos(pitch);
    const float sp = std::sin(pitch);
    return normalizeSafe({sy * cp, sp, cy * cp});
}

LightKind mapLightType(Light::Type type) {
    switch (type) {
        case Light::Directional: return LightKind::Directional;
        case Light::Point: return LightKind::Point;
        case Light::Spot: return LightKind::Spot;
        case Light::Area: return LightKind::Area;
    }
    return LightKind::Point;
}

} // namespace

void LightManager::gatherFromScene(Scene& scene) {
    m_lights.clear();

    scene.each<Light>([&](EntityId id, Light& light) {
        RuntimeLight runtime;
        runtime.kind = mapLightType(light.type);
        runtime.color = light.color;
        runtime.intensity = std::max(0.0f, light.intensity);
        runtime.castShadows = light.castShadows;
        runtime.volumetric = light.volumetric;
        runtime.range = std::max(0.1f, light.range);
        runtime.innerConeCos = std::cos(std::clamp(light.innerConeDeg, 0.1f, 89.0f) * DEG2RAD);
        runtime.outerConeCos = std::cos(std::clamp(light.outerConeDeg, 0.1f, 89.9f) * DEG2RAD);
        runtime.attenuation = {
            std::max(0.0f, light.attenuationConstant),
            std::max(0.0f, light.attenuationLinear),
            std::max(0.0f, light.attenuationQuadratic)
        };

        if (auto* transform = scene.getComponent<Transform>(id)) {
            runtime.position = transform->position;
            runtime.direction = directionFromEulerDeg(transform->rotation);
        } else {
            runtime.direction = {0.0f, -1.0f, 0.0f};
        }

        m_lights.push_back(runtime);
    });

    if (m_lights.empty()) {
        RuntimeLight fallback;
        fallback.kind = LightKind::Directional;
        fallback.direction = normalizeSafe({-0.35f, -1.0f, -0.2f});
        fallback.color = {1.0f, 0.98f, 0.95f};
        fallback.intensity = 3.0f;
        fallback.castShadows = true;
        m_lights.push_back(fallback);
    }
}

const RuntimeLight* LightManager::mainDirectionalLight() const {
    for (const auto& light : m_lights) {
        if (light.kind == LightKind::Directional) {
            return &light;
        }
    }
    return nullptr;
}

} // namespace rex::gfx
