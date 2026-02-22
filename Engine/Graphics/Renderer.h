#pragma once

#include "../Core/Components.h"
#include "../Core/Scene.h"
#include "Pipeline/DeferredPipeline.h"

#include <functional>
#include <memory>

namespace rex {

class Renderer {
public:
    Renderer();
    ~Renderer();

    void render(Scene& scene,
                const Camera& camera,
                const Mat4& viewMatrix,
                const Vec3& viewPos,
                int targetWidth,
                int targetHeight,
                uint32_t backbufferFBO,
                const std::function<void(const Mat4&, const Mat4&)>& extraDraw = {});

    gfx::DeferredPipeline& deferredPipeline() { return *m_deferred; }

private:
    std::unique_ptr<gfx::DeferredPipeline> m_deferred;
};

} // namespace rex
