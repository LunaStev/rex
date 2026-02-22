#include "RenderDevice.h"

namespace rex::gfx {

void RenderDevice::bindFramebuffer(uint32_t fbo) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void RenderDevice::setViewport(int x, int y, int w, int h) {
    glViewport(x, y, w, h);
}

void RenderDevice::clearColorDepth(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderDevice::clearDepth() {
    glClear(GL_DEPTH_BUFFER_BIT);
}

void RenderDevice::setDepthTest(bool enabled) {
    if (enabled) glEnable(GL_DEPTH_TEST);
    else glDisable(GL_DEPTH_TEST);
}

void RenderDevice::setCullFace(bool enabled) {
    if (enabled) {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    } else {
        glDisable(GL_CULL_FACE);
    }
}

} // namespace rex::gfx
