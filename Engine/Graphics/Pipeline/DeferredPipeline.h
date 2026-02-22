#pragma once

#include "../Core/RenderGraph.h"
#include "../Core/FrameBuffer.h"
#include "../Culling/FrustumCuller.h"
#include "../Culling/LightCuller.h"
#include "../HDR/HDRBuffer.h"
#include "../Lighting/LightManager.h"
#include "../Lighting/ShadowSystem.h"
#include "../PostProcess/SSAOPass.h"
#include "PostProcessPipeline.h"

#include <array>
#include <memory>
#include <vector>

namespace rex::gfx {

class DeferredPipeline {
public:
    DeferredPipeline();
    ~DeferredPipeline();

    void render(RenderFrameContext& ctx);

    PostProcessPipeline& postProcess() { return m_postProcess; }
    const PostProcessPipeline& postProcess() const { return m_postProcess; }

private:
    class ShadowPass : public RenderPass {
    public:
        explicit ShadowPass(DeferredPipeline& owner)
            : m_owner(owner) {
        }
        const char* name() const override { return "ShadowPass"; }
        void execute(RenderFrameContext& ctx) override;

    private:
        DeferredPipeline& m_owner;
    };

    class GBufferPass : public RenderPass {
    public:
        explicit GBufferPass(DeferredPipeline& owner)
            : m_owner(owner) {
        }
        const char* name() const override { return "GBufferPass"; }
        void execute(RenderFrameContext& ctx) override;

    private:
        DeferredPipeline& m_owner;
    };

    class LightingPass : public RenderPass {
    public:
        explicit LightingPass(DeferredPipeline& owner)
            : m_owner(owner) {
        }
        const char* name() const override { return "LightingPass"; }
        void execute(RenderFrameContext& ctx) override;

    private:
        DeferredPipeline& m_owner;
    };

    class PostProcessPass : public RenderPass {
    public:
        explicit PostProcessPass(DeferredPipeline& owner)
            : m_owner(owner) {
        }
        const char* name() const override { return "PostProcessPass"; }
        void execute(RenderFrameContext& ctx) override;

    private:
        DeferredPipeline& m_owner;
    };

    class UIPass : public RenderPass {
    public:
        explicit UIPass(DeferredPipeline& owner)
            : m_owner(owner) {
        }
        const char* name() const override { return "UIPass"; }
        void execute(RenderFrameContext& ctx) override;

    private:
        DeferredPipeline& m_owner;
    };

    void ensureResources(int width, int height);
    void initShaders();
    void initScreenTriangle();

    Vec3 cameraForward(const Mat4& viewMatrix) const;

    void executeShadowPass(RenderFrameContext& ctx);
    void executeGBufferPass(RenderFrameContext& ctx);
    void executeLightingPass(RenderFrameContext& ctx);
    void executePostProcessPass(RenderFrameContext& ctx);
    void executeUiPass(RenderFrameContext& ctx);

    void bindLightUniforms(const std::vector<RuntimeLight>& lights,
                           const std::array<Mat4, ShadowSystem::kMaxCascades>& cascadeMatrices,
                           const std::array<Vec4, ShadowSystem::kMaxCascades>& cascadeRects,
                           const std::array<float, ShadowSystem::kMaxCascades>& cascadeSplits,
                           float shadowAtlasResolution,
                           const Vec3& viewPos);

    RenderGraph m_graph;

    LightManager m_lightManager;
    LightCuller m_lightCuller;
    FrustumCuller m_frustumCuller;
    ShadowSystem m_shadowSystem;

    PostProcessPipeline m_postProcess;
    SSAOPass m_ssaoPass;

    FrameBuffer m_gbuffer;
    HDRBuffer m_lightingBuffer;

    std::unique_ptr<Shader> m_gbufferShader;
    std::unique_ptr<Shader> m_lightingShader;

    uint32_t m_screenVAO = 0;
    uint32_t m_screenVBO = 0;

    uint32_t m_ssaoTexture = 0;
    uint32_t m_whiteTexture = 0;
    bool m_enableSSAO = false;

    int m_width = 0;
    int m_height = 0;

    std::vector<VisibleRenderable> m_visible;
    std::vector<RuntimeLight> m_activeLights;

    ShadowPass m_shadowPass;
    GBufferPass m_gbufferPass;
    LightingPass m_lightingPass;
    PostProcessPass m_postProcessPass;
    UIPass m_uiPass;
};

} // namespace rex::gfx
