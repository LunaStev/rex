#include "DeferredPipeline.h"

#include "../Core/RenderDevice.h"
#include "../Model.h"

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

namespace rex::gfx {

namespace {

Vec3 normalizeSafe(const Vec3& v) {
    const float lenSq = dot(v, v);
    if (lenSq <= 1e-8f) return {0.0f, 0.0f, 1.0f};
    const float invLen = 1.0f / std::sqrt(lenSq);
    return {v.x * invLen, v.y * invLen, v.z * invLen};
}

} // namespace

void DeferredPipeline::ShadowPass::execute(RenderFrameContext& ctx) {
    m_owner.executeShadowPass(ctx);
}

void DeferredPipeline::GBufferPass::execute(RenderFrameContext& ctx) {
    m_owner.executeGBufferPass(ctx);
}

void DeferredPipeline::LightingPass::execute(RenderFrameContext& ctx) {
    m_owner.executeLightingPass(ctx);
}

void DeferredPipeline::PostProcessPass::execute(RenderFrameContext& ctx) {
    m_owner.executePostProcessPass(ctx);
}

void DeferredPipeline::UIPass::execute(RenderFrameContext& ctx) {
    m_owner.executeUiPass(ctx);
}

DeferredPipeline::DeferredPipeline()
    : m_shadowPass(*this)
    , m_gbufferPass(*this)
    , m_lightingPass(*this)
    , m_postProcessPass(*this)
    , m_uiPass(*this) {
    initScreenTriangle();
    initShaders();

    glGenTextures(1, &m_whiteTexture);
    glBindTexture(GL_TEXTURE_2D, m_whiteTexture);
    const float white[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1, 1, 0, GL_RGBA, GL_FLOAT, white);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

DeferredPipeline::~DeferredPipeline() {
    if (m_screenVBO) glDeleteBuffers(1, &m_screenVBO);
    if (m_screenVAO) glDeleteVertexArrays(1, &m_screenVAO);
    if (m_whiteTexture) glDeleteTextures(1, &m_whiteTexture);
}

void DeferredPipeline::initScreenTriangle() {
    const float verts[] = {
        -1.0f, -1.0f, 0.0f, 0.0f,
        3.0f, -1.0f, 2.0f, 0.0f,
        -1.0f, 3.0f, 0.0f, 2.0f,
    };

    glGenVertexArrays(1, &m_screenVAO);
    glGenBuffers(1, &m_screenVBO);
    glBindVertexArray(m_screenVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_screenVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);
}

void DeferredPipeline::initShaders() {
    const std::string gbufferVS = R"(
        #version 450 core
        layout(location = 0) in vec3 aPos;
        layout(location = 1) in vec3 aNormal;
        layout(location = 2) in vec2 aUV;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 proj;

        out vec3 vWorldPos;
        out vec3 vNormal;

        void main() {
            vec4 world = model * vec4(aPos, 1.0);
            vWorldPos = world.xyz;
            vNormal = mat3(transpose(inverse(model))) * aNormal;
            gl_Position = proj * view * world;
        }
    )";

    const std::string gbufferFS = R"(
        #version 450 core

        layout(location = 0) out vec4 gPosition;
        layout(location = 1) out vec4 gNormalRoughness;
        layout(location = 2) out vec4 gAlbedoMetallic;
        layout(location = 3) out vec4 gAO;

        in vec3 vWorldPos;
        in vec3 vNormal;

        uniform vec3 uAlbedo;
        uniform float uRoughness;
        uniform float uMetallic;
        uniform float uAO;

        void main() {
            vec3 n = normalize(vNormal);
            gPosition = vec4(vWorldPos, 1.0);
            gNormalRoughness = vec4(n * 0.5 + 0.5, clamp(uRoughness, 0.04, 1.0));
            gAlbedoMetallic = vec4(max(uAlbedo, vec3(0.0)), clamp(uMetallic, 0.0, 1.0));
            gAO = vec4(clamp(uAO, 0.0, 1.0), 0.0, 0.0, 1.0);
        }
    )";

    const std::string lightingVS = R"(
        #version 450 core
        layout(location = 0) in vec2 aPos;
        layout(location = 1) in vec2 aUV;

        out vec2 vUV;

        void main() {
            vUV = aUV;
            gl_Position = vec4(aPos, 0.0, 1.0);
        }
    )";

    const std::string lightingFS = R"(
        #version 450 core

        in vec2 vUV;
        out vec4 FragColor;

        uniform sampler2D uGPosition;
        uniform sampler2D uGNormalRoughness;
        uniform sampler2D uGAlbedoMetallic;
        uniform sampler2D uGAO;
        uniform sampler2D uSSAO;
        uniform sampler2D uShadowAtlas;

        uniform vec3 uViewPos;
        uniform int uUseSSAO;

        uniform int uLightCount;
        uniform int uLightType[128];
        uniform vec3 uLightPos[128];
        uniform vec3 uLightDir[128];
        uniform vec3 uLightColor[128];
        uniform float uLightIntensity[128];
        uniform float uLightRange[128];
        uniform vec3 uLightAtten[128];
        uniform float uSpotInnerCos[128];
        uniform float uSpotOuterCos[128];
        uniform int uLightCastShadow[128];

        uniform mat4 uCascadeMatrices[4];
        uniform vec3 uCascadeRectMin[4];
        uniform vec3 uCascadeRectSize[4];
        uniform float uCascadeSplits[4];
        uniform float uShadowAtlasResolution;

        const float PI = 3.14159265359;

        float distributionGGX(vec3 N, vec3 H, float roughness) {
            float a = roughness * roughness;
            float a2 = a * a;
            float NdotH = max(dot(N, H), 0.0);
            float NdotH2 = NdotH * NdotH;

            float numerator = a2;
            float denominator = (NdotH2 * (a2 - 1.0) + 1.0);
            denominator = PI * denominator * denominator;
            return numerator / max(denominator, 0.0001);
        }

        float geometrySchlickGGX(float NdotV, float roughness) {
            float r = roughness + 1.0;
            float k = (r * r) / 8.0;
            float numerator = NdotV;
            float denominator = NdotV * (1.0 - k) + k;
            return numerator / max(denominator, 0.0001);
        }

        float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
            float NdotV = max(dot(N, V), 0.0);
            float NdotL = max(dot(N, L), 0.0);
            float ggx2 = geometrySchlickGGX(NdotV, roughness);
            float ggx1 = geometrySchlickGGX(NdotL, roughness);
            return ggx1 * ggx2;
        }

        vec3 fresnelSchlick(float cosTheta, vec3 F0) {
            return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
        }

        int selectCascade(vec3 worldPos) {
            float d = length(worldPos - uViewPos);
            if (d <= uCascadeSplits[0]) return 0;
            if (d <= uCascadeSplits[1]) return 1;
            if (d <= uCascadeSplits[2]) return 2;
            return 3;
        }

        float sampleShadowAtlas(vec3 worldPos, vec3 normal, vec3 L) {
            int cascade = selectCascade(worldPos);

            vec4 lightClip = uCascadeMatrices[cascade] * vec4(worldPos, 1.0);
            vec3 proj = lightClip.xyz / max(lightClip.w, 0.0001);
            proj = proj * 0.5 + 0.5;

            if (proj.x < 0.0 || proj.x > 1.0 || proj.y < 0.0 || proj.y > 1.0 || proj.z > 1.0) {
                return 1.0;
            }

            vec2 rectMin = uCascadeRectMin[cascade].xy;
            vec2 rectSize = uCascadeRectSize[cascade].xy;
            vec2 atlasUV = rectMin + proj.xy * rectSize;

            float bias = max(0.0005, 0.002 * (1.0 - max(dot(normal, L), 0.0)));
            float currentDepth = proj.z - bias;

            float texel = 1.0 / max(uShadowAtlasResolution, 1.0);
            float sum = 0.0;
            int samples = 0;

            for (int x = -1; x <= 1; ++x) {
                for (int y = -1; y <= 1; ++y) {
                    vec2 offset = vec2(float(x), float(y)) * texel;
                    float sampledDepth = texture(uShadowAtlas, atlasUV + offset).r;
                    sum += currentDepth <= sampledDepth ? 1.0 : 0.0;
                    samples++;
                }
            }

            return sum / float(max(samples, 1));
        }

        void main() {
            vec3 worldPos = texture(uGPosition, vUV).xyz;
            vec4 nR = texture(uGNormalRoughness, vUV);
            vec4 aM = texture(uGAlbedoMetallic, vUV);

            vec3 N = normalize(nR.xyz * 2.0 - 1.0);
            float roughness = clamp(nR.w, 0.04, 1.0);
            vec3 albedo = max(aM.rgb, vec3(0.0));
            float metallic = clamp(aM.w, 0.0, 1.0);

            float materialAO = clamp(texture(uGAO, vUV).r, 0.0, 1.0);
            float ssao = (uUseSSAO == 1) ? clamp(texture(uSSAO, vUV).r, 0.0, 1.0) : 1.0;
            float ao = materialAO * ssao;

            vec3 V = normalize(uViewPos - worldPos);
            vec3 F0 = mix(vec3(0.04), albedo, metallic);

            vec3 Lo = vec3(0.0);

            for (int i = 0; i < uLightCount; ++i) {
                vec3 L = vec3(0.0);
                float attenuation = 1.0;

                if (uLightType[i] == 0) {
                    L = normalize(-uLightDir[i]);
                } else {
                    vec3 toLight = uLightPos[i] - worldPos;
                    float dist = max(length(toLight), 0.001);
                    L = toLight / dist;

                    float c = uLightAtten[i].x;
                    float l = uLightAtten[i].y;
                    float q = uLightAtten[i].z;
                    attenuation = 1.0 / max(c + l * dist + q * dist * dist, 0.0001);

                    float rangeFade = clamp(1.0 - dist / max(uLightRange[i], 0.001), 0.0, 1.0);
                    attenuation *= rangeFade * rangeFade;

                    if (uLightType[i] == 2) {
                        float cosTheta = dot(normalize(-uLightDir[i]), L);
                        float inner = uSpotInnerCos[i];
                        float outer = min(inner - 0.0001, uSpotOuterCos[i]);
                        float spot = clamp((cosTheta - outer) / max(inner - outer, 0.0001), 0.0, 1.0);
                        attenuation *= spot;
                    }
                }

                vec3 H = normalize(V + L);
                float NdotL = max(dot(N, L), 0.0);
                float NdotV = max(dot(N, V), 0.0);

                float NDF = distributionGGX(N, H, roughness);
                float G = geometrySmith(N, V, L, roughness);
                vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

                vec3 numerator = NDF * G * F;
                float denominator = 4.0 * NdotV * NdotL + 0.0001;
                vec3 specular = numerator / denominator;

                vec3 kS = F;
                vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);

                vec3 radiance = uLightColor[i] * uLightIntensity[i] * attenuation;

                float shadow = 1.0;
                if (uLightType[i] == 0 && uLightCastShadow[i] == 1) {
                    shadow = sampleShadowAtlas(worldPos, N, L);
                }

                Lo += (kD * albedo / PI + specular) * radiance * NdotL * shadow;
            }

            vec3 skyTint = vec3(0.05, 0.07, 0.09);
            vec3 ambientDiffuse = vec3(0.03) * albedo * ao;
            vec3 ambientSpec = skyTint * (F0 * (1.0 - roughness));

            vec3 color = ambientDiffuse + ambientSpec + Lo;
            FragColor = vec4(max(color, vec3(0.0)), 1.0);
        }
    )";

    m_gbufferShader = std::make_unique<Shader>(gbufferVS, gbufferFS);
    m_lightingShader = std::make_unique<Shader>(lightingVS, lightingFS);
}

void DeferredPipeline::ensureResources(int width, int height) {
    if (width <= 0 || height <= 0) return;

    if (m_width == width && m_height == height && m_gbuffer.isValid()) {
        m_lightingBuffer.ensure(width, height);
        m_ssaoPass.ensure(width, height);
        m_shadowSystem.ensureResources(2048);
        return;
    }

    m_width = width;
    m_height = height;

    std::vector<ColorAttachmentDesc> gbufferAttachments;

    ColorAttachmentDesc gPosition;
    gPosition.internalFormat = GL_RGBA16F;
    gPosition.format = GL_RGBA;
    gPosition.type = GL_FLOAT;
    gbufferAttachments.push_back(gPosition);

    ColorAttachmentDesc gNormalRoughness;
    gNormalRoughness.internalFormat = GL_RGBA16F;
    gNormalRoughness.format = GL_RGBA;
    gNormalRoughness.type = GL_FLOAT;
    gbufferAttachments.push_back(gNormalRoughness);

    ColorAttachmentDesc gAlbedoMetallic;
    gAlbedoMetallic.internalFormat = GL_RGBA16F;
    gAlbedoMetallic.format = GL_RGBA;
    gAlbedoMetallic.type = GL_FLOAT;
    gbufferAttachments.push_back(gAlbedoMetallic);

    ColorAttachmentDesc gAo;
    gAo.internalFormat = GL_R16F;
    gAo.format = GL_RED;
    gAo.type = GL_FLOAT;
    gAo.minFilter = GL_LINEAR;
    gAo.magFilter = GL_LINEAR;
    gbufferAttachments.push_back(gAo);

    m_gbuffer.create(width,
                     height,
                     gbufferAttachments,
                     DepthAttachmentMode::Texture,
                     GL_DEPTH_COMPONENT24);

    m_lightingBuffer.ensure(width, height);
    m_ssaoPass.ensure(width, height);
    m_shadowSystem.ensureResources(2048);
}

Vec3 DeferredPipeline::cameraForward(const Mat4& viewMatrix) const {
    return normalizeSafe({viewMatrix.m[2], viewMatrix.m[6], viewMatrix.m[10]});
}

void DeferredPipeline::render(RenderFrameContext& ctx) {
    if (ctx.targetWidth <= 0 || ctx.targetHeight <= 0) return;

    ensureResources(ctx.targetWidth, ctx.targetHeight);

    m_lightManager.gatherFromScene(ctx.scene);
    m_activeLights = m_lightCuller.cullForView(m_lightManager.lights(), ctx.viewPos, 128);

    m_visible = m_frustumCuller.collectVisible(ctx.scene,
                                               ctx.viewPos,
                                               cameraForward(ctx.viewMatrix),
                                               ctx.camera.fov,
                                               ctx.camera.aspect,
                                               ctx.camera.nearPlane,
                                               ctx.camera.farPlane);

    m_graph.clear();
    m_graph.addPass(m_shadowPass);
    m_graph.addPass(m_gbufferPass);
    m_graph.addPass(m_lightingPass);
    m_graph.addPass(m_postProcessPass);
    m_graph.addPass(m_uiPass);
    m_graph.execute(ctx);
}

void DeferredPipeline::executeShadowPass(RenderFrameContext& ctx) {
    const RuntimeLight* directional = nullptr;
    for (const auto& light : m_activeLights) {
        if (light.kind == LightKind::Directional && light.castShadows) {
            directional = &light;
            break;
        }
    }

    m_shadowSystem.renderCascades(ctx.scene, ctx.camera, ctx.viewMatrix, ctx.viewPos, directional);
}

void DeferredPipeline::executeGBufferPass(RenderFrameContext& ctx) {
    RenderDevice::bindFramebuffer(m_gbuffer.id());
    RenderDevice::setViewport(0, 0, m_width, m_height);
    RenderDevice::clearColorDepth(0.0f, 0.0f, 0.0f, 1.0f);
    RenderDevice::setDepthTest(true);
    RenderDevice::setCullFace(true);

    m_gbufferShader->bind();
    m_gbufferShader->setUniform("view", ctx.viewMatrix);
    m_gbufferShader->setUniform("proj", ctx.projMatrix);

    for (const auto& item : m_visible) {
        if (!item.transform || !item.renderer) continue;

        const Mat4 model = item.transform->getMatrix();
        m_gbufferShader->setUniform("model", model);
        m_gbufferShader->setUniform("uAlbedo", item.renderer->color);
        m_gbufferShader->setUniform("uRoughness", item.renderer->roughness);
        m_gbufferShader->setUniform("uMetallic", item.renderer->metallic);
        m_gbufferShader->setUniform("uAO", item.renderer->ao);

        if (item.renderer->model) {
            item.renderer->model->draw();
        } else if (item.renderer->mesh) {
            item.renderer->mesh->draw();
        }
    }

    RenderDevice::setCullFace(false);
}

void DeferredPipeline::bindLightUniforms(const std::vector<RuntimeLight>& lights,
                                         const std::array<Mat4, ShadowSystem::kMaxCascades>& cascadeMatrices,
                                         const std::array<Vec4, ShadowSystem::kMaxCascades>& cascadeRects,
                                         const std::array<float, ShadowSystem::kMaxCascades>& cascadeSplits,
                                         float shadowAtlasResolution,
                                         const Vec3& viewPos) {
    m_lightingShader->setUniform("uViewPos", viewPos);
    m_lightingShader->setUniform("uLightCount", static_cast<int>(lights.size()));

    for (int i = 0; i < ShadowSystem::kMaxCascades; ++i) {
        m_lightingShader->setUniform("uCascadeMatrices[" + std::to_string(i) + "]", cascadeMatrices[i]);
        m_lightingShader->setUniform("uCascadeRectMin[" + std::to_string(i) + "]", Vec3{cascadeRects[i].x, cascadeRects[i].y, 0.0f});
        m_lightingShader->setUniform("uCascadeRectSize[" + std::to_string(i) + "]", Vec3{cascadeRects[i].z, cascadeRects[i].w, 0.0f});
        m_lightingShader->setUniform("uCascadeSplits[" + std::to_string(i) + "]", cascadeSplits[i]);
    }
    m_lightingShader->setUniform("uShadowAtlasResolution", shadowAtlasResolution);

    for (int i = 0; i < static_cast<int>(lights.size()); ++i) {
        const auto& l = lights[static_cast<size_t>(i)];
        const int type = static_cast<int>(l.kind);

        m_lightingShader->setUniform("uLightType[" + std::to_string(i) + "]", type);
        m_lightingShader->setUniform("uLightPos[" + std::to_string(i) + "]", l.position);
        m_lightingShader->setUniform("uLightDir[" + std::to_string(i) + "]", l.direction);
        m_lightingShader->setUniform("uLightColor[" + std::to_string(i) + "]", l.color);
        m_lightingShader->setUniform("uLightIntensity[" + std::to_string(i) + "]", l.intensity);
        m_lightingShader->setUniform("uLightRange[" + std::to_string(i) + "]", l.range);
        m_lightingShader->setUniform("uLightAtten[" + std::to_string(i) + "]", l.attenuation);
        m_lightingShader->setUniform("uSpotInnerCos[" + std::to_string(i) + "]", l.innerConeCos);
        m_lightingShader->setUniform("uSpotOuterCos[" + std::to_string(i) + "]", l.outerConeCos);
        m_lightingShader->setUniform("uLightCastShadow[" + std::to_string(i) + "]", l.castShadows ? 1 : 0);
    }
}

void DeferredPipeline::executeLightingPass(RenderFrameContext& ctx) {
    if (m_enableSSAO) {
        m_ssaoTexture = m_ssaoPass.execute(m_gbuffer.colorTexture(0),
                                           m_gbuffer.colorTexture(1),
                                           ctx.projMatrix,
                                           m_screenVAO,
                                           m_width,
                                           m_height);
    } else {
        m_ssaoTexture = 0;
    }

    RenderDevice::bindFramebuffer(m_lightingBuffer.framebuffer());
    RenderDevice::setViewport(0, 0, m_width, m_height);
    RenderDevice::clearColorDepth(0.0f, 0.0f, 0.0f, 1.0f);
    RenderDevice::setDepthTest(false);

    m_lightingShader->bind();
    m_lightingShader->setUniform("uGPosition", 0);
    m_lightingShader->setUniform("uGNormalRoughness", 1);
    m_lightingShader->setUniform("uGAlbedoMetallic", 2);
    m_lightingShader->setUniform("uGAO", 3);
    m_lightingShader->setUniform("uSSAO", 4);
    m_lightingShader->setUniform("uShadowAtlas", 5);
    m_lightingShader->setUniform("uUseSSAO", m_ssaoTexture ? 1 : 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_gbuffer.colorTexture(0));
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_gbuffer.colorTexture(1));
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_gbuffer.colorTexture(2));
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, m_gbuffer.colorTexture(3));
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, m_ssaoTexture ? m_ssaoTexture : m_whiteTexture);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, m_shadowSystem.shadowAtlasTexture() ? m_shadowSystem.shadowAtlasTexture() : m_whiteTexture);

    bindLightUniforms(m_activeLights,
                      m_shadowSystem.cascadeMatrices(),
                      m_shadowSystem.cascadeAtlasRects(),
                      m_shadowSystem.cascadeSplits(),
                      static_cast<float>(m_shadowSystem.atlasResolution()),
                      ctx.viewPos);

    glBindVertexArray(m_screenVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    if (ctx.extraDraw) {
        ctx.extraDraw(ctx.viewMatrix, ctx.projMatrix);
    }
}

void DeferredPipeline::executePostProcessPass(RenderFrameContext& ctx) {
    m_postProcess.execute(m_lightingBuffer.colorTexture(),
                          ctx.backbufferFBO,
                          m_width,
                          m_height,
                          m_screenVAO,
                          1.0f / 60.0f,
                          1.0f,
                          0);
}

void DeferredPipeline::executeUiPass(RenderFrameContext& ctx) {
    (void)ctx;
    // Reserved for UI composition pass.
}

} // namespace rex::gfx
