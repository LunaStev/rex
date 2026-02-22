#pragma once

#include "../../Core/Components.h"
#include "../../Core/Scene.h"

#include <cstdint>
#include <functional>

namespace rex::gfx {

struct RenderFrameContext {
    Scene& scene;
    const Camera& camera;
    Mat4 viewMatrix;
    Mat4 projMatrix;
    Vec3 viewPos;
    int targetWidth = 0;
    int targetHeight = 0;
    uint32_t backbufferFBO = 0;
    std::function<void(const Mat4&, const Mat4&)> extraDraw;
};

class RenderPass {
public:
    virtual ~RenderPass() = default;

    virtual const char* name() const = 0;
    virtual void execute(RenderFrameContext& ctx) = 0;
};

} // namespace rex::gfx
