#include "RenderGraph.h"

namespace rex::gfx {

void RenderGraph::addPass(RenderPass& pass) {
    m_passes.push_back(&pass);
}

void RenderGraph::clear() {
    m_passes.clear();
}

void RenderGraph::execute(RenderFrameContext& ctx) const {
    for (RenderPass* pass : m_passes) {
        if (!pass) continue;
        pass->execute(ctx);
    }
}

} // namespace rex::gfx
