#include "HDRBuffer.h"

namespace rex::gfx {

bool HDRBuffer::ensure(int width, int height) {
    if (width <= 0 || height <= 0) return false;
    if (m_buffer.isValid() && m_buffer.width() == width && m_buffer.height() == height) {
        return true;
    }

    std::vector<ColorAttachmentDesc> attachments;
    attachments.push_back(ColorAttachmentDesc{});
    return m_buffer.create(width, height, attachments, DepthAttachmentMode::Renderbuffer, GL_DEPTH_COMPONENT24);
}

void HDRBuffer::destroy() {
    m_buffer.destroy();
}

} // namespace rex::gfx
