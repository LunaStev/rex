#pragma once

#include "../../Graphics/Shader.h"
#include "../Core/FrameBuffer.h"
#include "LightManager.h"

#include <array>
#include <memory>

namespace rex::gfx {

class ShadowSystem {
public:
    static constexpr int kMaxCascades = 4;

    ShadowSystem();
    ~ShadowSystem() = default;

    void ensureResources(int atlasResolution);
    void renderCascades(Scene& scene,
                        const Camera& camera,
                        const Mat4& viewMatrix,
                        const Vec3& viewPos,
                        const RuntimeLight* directionalLight);

    uint32_t shadowAtlasTexture() const { return m_shadowAtlas.depthTexture(); }
    int atlasResolution() const { return m_atlasResolution; }

    const std::array<Mat4, kMaxCascades>& cascadeMatrices() const { return m_lightViewProj; }
    const std::array<Vec4, kMaxCascades>& cascadeAtlasRects() const { return m_atlasRects; }
    const std::array<float, kMaxCascades>& cascadeSplits() const { return m_cascadeSplits; }

private:
    Mat4 makeOrtho(float left, float right, float bottom, float top, float zNear, float zFar) const;
    Vec3 normalizeSafe(const Vec3& v) const;

    FrameBuffer m_shadowAtlas;
    std::unique_ptr<Shader> m_depthShader;

    int m_atlasResolution = 2048;

    std::array<Mat4, kMaxCascades> m_lightViewProj{};
    std::array<Vec4, kMaxCascades> m_atlasRects{};
    std::array<float, kMaxCascades> m_cascadeSplits{};
};

} // namespace rex::gfx
