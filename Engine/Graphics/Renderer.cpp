#include "Renderer.h"

namespace rex {

Renderer::Renderer() {
    m_deferred = std::make_unique<gfx::DeferredPipeline>();
}

Renderer::~Renderer() = default;

void Renderer::render(Scene& scene,
                      const Camera& camera,
                      const Mat4& viewMatrix,
                      const Vec3& viewPos,
                      int targetWidth,
                      int targetHeight,
                      uint32_t backbufferFBO,
                      const std::function<void(const Mat4&, const Mat4&)>& extraDraw) {
    if (!glUseProgram) return;

    gfx::RenderFrameContext ctx{
        scene,
        camera,
        viewMatrix,
        camera.getProjection(),
        viewPos,
        targetWidth,
        targetHeight,
        backbufferFBO,
        extraDraw,
    };

    m_deferred->render(ctx);
}

} // namespace rex
