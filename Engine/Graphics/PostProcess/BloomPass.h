#pragma once

#include "../../Graphics/Shader.h"
#include "../Core/FrameBuffer.h"

#include <memory>

namespace rex::gfx {

class BloomPass {
public:
    BloomPass();

    bool ensure(int width, int height);
    uint32_t execute(uint32_t hdrTexture, uint32_t screenVAO);

private:
    FrameBuffer m_brightPass;
    FrameBuffer m_ping;
    FrameBuffer m_pong;

    std::unique_ptr<Shader> m_extractShader;
    std::unique_ptr<Shader> m_blurShader;

    int m_width = 0;
    int m_height = 0;
};

} // namespace rex::gfx
