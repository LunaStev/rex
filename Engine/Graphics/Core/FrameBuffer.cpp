#include "FrameBuffer.h"

#include <iostream>

namespace rex::gfx {

FrameBuffer::~FrameBuffer() {
    destroy();
}

uint32_t FrameBuffer::colorTexture(size_t idx) const {
    if (idx >= m_colorTextures.size()) return 0;
    return m_colorTextures[idx];
}

bool FrameBuffer::create(int width,
                         int height,
                         const std::vector<ColorAttachmentDesc>& colorAttachments,
                         DepthAttachmentMode depthMode,
                         uint32_t depthInternalFormat) {
    if (width <= 0 || height <= 0) return false;

    destroy();

    m_width = width;
    m_height = height;

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    m_colorTextures.reserve(colorAttachments.size());

    std::vector<uint32_t> drawBuffers;
    drawBuffers.reserve(colorAttachments.size());

    for (size_t i = 0; i < colorAttachments.size(); ++i) {
        const auto& desc = colorAttachments[i];

        uint32_t tex = 0;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     static_cast<int>(desc.internalFormat),
                     width,
                     height,
                     0,
                     desc.format,
                     desc.type,
                     nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, desc.minFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, desc.magFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, desc.wrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, desc.wrapT);

        const uint32_t attachment = GL_COLOR_ATTACHMENT0 + static_cast<uint32_t>(i);
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, tex, 0);

        m_colorTextures.push_back(tex);
        drawBuffers.push_back(attachment);
    }

    if (!drawBuffers.empty()) {
        glDrawBuffers(static_cast<int>(drawBuffers.size()), drawBuffers.data());
    } else {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    if (depthMode == DepthAttachmentMode::Texture) {
        glGenTextures(1, &m_depthTexture);
        glBindTexture(GL_TEXTURE_2D, m_depthTexture);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     static_cast<int>(depthInternalFormat),
                     width,
                     height,
                     0,
                     GL_DEPTH_COMPONENT,
                     GL_FLOAT,
                     nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);
    } else if (depthMode == DepthAttachmentMode::Renderbuffer) {
        glGenRenderbuffers(1, &m_depthRenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, depthInternalFormat, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRenderbuffer);
    }

    const uint32_t status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "[FrameBuffer] Incomplete framebuffer: " << status << "\n";
        destroy();
        return false;
    }

    return true;
}

void FrameBuffer::destroy() {
    if (m_depthRenderbuffer) {
        glDeleteRenderbuffers(1, &m_depthRenderbuffer);
        m_depthRenderbuffer = 0;
    }

    if (m_depthTexture) {
        glDeleteTextures(1, &m_depthTexture);
        m_depthTexture = 0;
    }

    if (!m_colorTextures.empty()) {
        glDeleteTextures(static_cast<int>(m_colorTextures.size()), m_colorTextures.data());
        m_colorTextures.clear();
    }

    if (m_fbo) {
        glDeleteFramebuffers(1, &m_fbo);
        m_fbo = 0;
    }

    m_width = 0;
    m_height = 0;
}

} // namespace rex::gfx
