#include "ShadowSystem.h"

#include "../Model.h"

#include <algorithm>
#include <cmath>

namespace rex::gfx {

namespace {

constexpr float kCascadeLambda = 0.7f;

} // namespace

ShadowSystem::ShadowSystem() {
    const std::string vs = R"(
        #version 450 core
        layout(location = 0) in vec3 aPos;

        uniform mat4 model;
        uniform mat4 lightViewProj;

        void main() {
            gl_Position = lightViewProj * model * vec4(aPos, 1.0);
        }
    )";

    const std::string fs = R"(
        #version 450 core
        void main() {
        }
    )";

    m_depthShader = std::make_unique<Shader>(vs, fs);

    for (int i = 0; i < kMaxCascades; ++i) {
        m_lightViewProj[i] = Mat4::identity();
        m_cascadeSplits[i] = 0.0f;
        const int x = i % 2;
        const int y = i / 2;
        m_atlasRects[i] = {x * 0.5f, y * 0.5f, 0.5f, 0.5f};
    }
}

Vec3 ShadowSystem::normalizeSafe(const Vec3& v) const {
    const float lenSq = dot(v, v);
    if (lenSq <= 1e-8f) return {0.0f, -1.0f, 0.0f};
    const float invLen = 1.0f / std::sqrt(lenSq);
    return {v.x * invLen, v.y * invLen, v.z * invLen};
}

Mat4 ShadowSystem::makeOrtho(float left, float right, float bottom, float top, float zNear, float zFar) const {
    Mat4 m = Mat4::identity();
    m.m[0] = 2.0f / (right - left);
    m.m[5] = 2.0f / (top - bottom);
    m.m[10] = 2.0f / (zFar - zNear);
    m.m[12] = -(right + left) / (right - left);
    m.m[13] = -(top + bottom) / (top - bottom);
    m.m[14] = -(zFar + zNear) / (zFar - zNear);
    return m;
}

void ShadowSystem::ensureResources(int atlasResolution) {
    atlasResolution = std::max(512, atlasResolution);
    if (m_shadowAtlas.isValid() && m_atlasResolution == atlasResolution) {
        return;
    }

    m_atlasResolution = atlasResolution;

    std::vector<ColorAttachmentDesc> noColor;
    m_shadowAtlas.create(m_atlasResolution,
                         m_atlasResolution,
                         noColor,
                         DepthAttachmentMode::Texture,
                         GL_DEPTH_COMPONENT24);

    glBindTexture(GL_TEXTURE_2D, m_shadowAtlas.depthTexture());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void ShadowSystem::renderCascades(Scene& scene,
                                  const Camera& camera,
                                  const Mat4& viewMatrix,
                                  const Vec3& viewPos,
                                  const RuntimeLight* directionalLight) {
    ensureResources(m_atlasResolution);

    if (!m_shadowAtlas.isValid()) {
        return;
    }

    const Vec3 lightDir = normalizeSafe(directionalLight ? directionalLight->direction : Vec3{-0.35f, -1.0f, -0.2f});

    const float nearPlane = std::max(0.001f, camera.nearPlane);
    const float farPlane = std::max(nearPlane + 1.0f, camera.farPlane);

    float lastSplit = nearPlane;
    for (int i = 0; i < kMaxCascades; ++i) {
        const float p = float(i + 1) / float(kMaxCascades);
        const float logSplit = nearPlane * std::pow(farPlane / nearPlane, p);
        const float uniSplit = nearPlane + (farPlane - nearPlane) * p;
        const float split = kCascadeLambda * logSplit + (1.0f - kCascadeLambda) * uniSplit;
        m_cascadeSplits[i] = split;

        const float mid = 0.5f * (lastSplit + split);
        const float radius = std::max(6.0f, split * 0.75f);

        Vec3 camForward = normalizeSafe({viewMatrix.m[2], viewMatrix.m[6], viewMatrix.m[10]});
        if (dot(camForward, camForward) <= 1e-6f) {
            camForward = {0.0f, 0.0f, 1.0f};
        }

        const Vec3 center = viewPos + camForward * mid;
        const Vec3 lightPos = center - lightDir * (radius * 3.0f + 20.0f);

        const Mat4 lightView = Mat4::lookAtLH(lightPos, center, {0.0f, 1.0f, 0.0f});
        const Mat4 lightProj = makeOrtho(-radius, radius, -radius, radius, 0.1f, radius * 8.0f + 40.0f);
        m_lightViewProj[i] = lightProj * lightView;

        lastSplit = split;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, m_shadowAtlas.id());
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glEnable(GL_DEPTH_TEST);
    glClearDepth(1.0);
    glClear(GL_DEPTH_BUFFER_BIT);

    m_depthShader->bind();

    const int tileRes = m_atlasResolution / 2;
    for (int cascade = 0; cascade < kMaxCascades; ++cascade) {
        const int tileX = cascade % 2;
        const int tileY = cascade / 2;
        glViewport(tileX * tileRes, tileY * tileRes, tileRes, tileRes);
        glClear(GL_DEPTH_BUFFER_BIT);

        m_depthShader->setUniform("lightViewProj", m_lightViewProj[cascade]);

        scene.each<MeshRenderer>([&](EntityId id, MeshRenderer& mr) {
            (void)mr;
            auto* transform = scene.getComponent<Transform>(id);
            if (!transform) return;

            const Mat4 model = transform->getMatrix();
            m_depthShader->setUniform("model", model);

            if (mr.model) {
                mr.model->draw();
            } else if (mr.mesh) {
                mr.mesh->draw();
            }
        });
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

} // namespace rex::gfx
