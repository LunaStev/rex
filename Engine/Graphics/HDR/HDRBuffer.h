#pragma once

#include "../Core/FrameBuffer.h"

namespace rex::gfx {

class HDRBuffer {
public:
    bool ensure(int width, int height);
    void destroy();

    uint32_t framebuffer() const { return m_buffer.id(); }
    uint32_t colorTexture() const { return m_buffer.colorTexture(0); }
    int width() const { return m_buffer.width(); }
    int height() const { return m_buffer.height(); }

private:
    FrameBuffer m_buffer;
};

} // namespace rex::gfx
