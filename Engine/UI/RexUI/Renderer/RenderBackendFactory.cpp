#include "RenderBackendFactory.h"

#include "OpenGL/RexUIRendererGL.h"
#include "RexGraphics/RexUIRendererRexGraphics.h"
#include "Vulkan/RexUIRendererVK.h"

namespace rex::ui::renderer {

std::unique_ptr<IRenderBackend> createRenderBackend(
    RenderBackendType type,
    IRenderDevice* renderDevice,
    rexgraphics::IRexGraphicsDevice* rexGraphicsDevice) {
    switch (type) {
        case RenderBackendType::OpenGL:
            return std::make_unique<opengl::RexUIRendererGL>(renderDevice);
        case RenderBackendType::Vulkan:
            return std::make_unique<vulkan::RexUIRendererVK>(renderDevice);
        case RenderBackendType::RexGraphics: {
            if (!rexGraphicsDevice) return nullptr;
            return std::make_unique<rexgraphics::RexUIRendererRexGraphics>(renderDevice, rexGraphicsDevice);
        }
    }
    return nullptr;
}

} // namespace rex::ui::renderer

