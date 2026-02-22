#pragma once

#include "RenderPass.h"

#include <vector>

namespace rex::gfx {

class RenderGraph {
public:
    void addPass(RenderPass& pass);
    void clear();
    void execute(RenderFrameContext& ctx) const;

private:
    std::vector<RenderPass*> m_passes;
};

} // namespace rex::gfx
