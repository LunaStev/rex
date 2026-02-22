#pragma once

#include "../GLInternal.h"

#include <cstddef>
#include <vector>

namespace rex::gfx {

struct ColorAttachmentDesc {
    uint32_t internalFormat = GL_RGBA16F;
    uint32_t format = GL_RGBA;
    uint32_t type = GL_FLOAT;
    int minFilter = GL_LINEAR;
    int magFilter = GL_LINEAR;
    int wrapS = GL_CLAMP_TO_EDGE;
    int wrapT = GL_CLAMP_TO_EDGE;
};

enum class DepthAttachmentMode {
    None,
    Renderbuffer,
    Texture,
};

class FrameBuffer {
public:
    FrameBuffer() = default;
    ~FrameBuffer();

    FrameBuffer(const FrameBuffer&) = delete;
    FrameBuffer& operator=(const FrameBuffer&) = delete;

    bool create(int width,
                int height,
                const std::vector<ColorAttachmentDesc>& colorAttachments,
                DepthAttachmentMode depthMode,
                uint32_t depthInternalFormat = GL_DEPTH_COMPONENT24);

    void destroy();

    bool isValid() const { return m_fbo != 0; }
    uint32_t id() const { return m_fbo; }
    int width() const { return m_width; }
    int height() const { return m_height; }

    size_t colorCount() const { return m_colorTextures.size(); }
    uint32_t colorTexture(size_t idx) const;
    uint32_t depthTexture() const { return m_depthTexture; }

private:
    uint32_t m_fbo = 0;
    std::vector<uint32_t> m_colorTextures;
    uint32_t m_depthTexture = 0;
    uint32_t m_depthRenderbuffer = 0;
    int m_width = 0;
    int m_height = 0;
};

} // namespace rex::gfx
