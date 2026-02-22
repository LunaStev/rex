#pragma once

#include "../Core/RenderPass.h"

namespace rex::gfx {

class ForwardPipeline {
public:
    // Placeholder for forward/forward+ path fallback.
    void render(RenderFrameContext& ctx);
};

} // namespace rex::gfx
