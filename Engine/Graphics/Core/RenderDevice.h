#pragma once

#include "../GLInternal.h"

namespace rex::gfx {

class RenderDevice {
public:
    static void bindFramebuffer(uint32_t fbo);
    static void setViewport(int x, int y, int w, int h);
    static void clearColorDepth(float r, float g, float b, float a);
    static void clearDepth();
    static void setDepthTest(bool enabled);
    static void setCullFace(bool enabled);
};

} // namespace rex::gfx
