#include "BloomPass.h"

#include <algorithm>

namespace rex::gfx {

BloomPass::BloomPass() {
    const std::string vs = R"(
        #version 450 core
        layout(location = 0) in vec2 aPos;
        layout(location = 1) in vec2 aUV;
        out vec2 vUV;
        void main() {
            vUV = aUV;
            gl_Position = vec4(aPos, 0.0, 1.0);
        }
    )";

    const std::string extractFs = R"(
        #version 450 core
        in vec2 vUV;
        out vec4 FragColor;

        uniform sampler2D uSource;
        uniform float uThreshold;

        void main() {
            vec3 color = texture(uSource, vUV).rgb;
            float lum = dot(color, vec3(0.2126, 0.7152, 0.0722));
            vec3 bright = lum > uThreshold ? color : vec3(0.0);
            FragColor = vec4(bright, 1.0);
        }
    )";

    const std::string blurFs = R"(
        #version 450 core
        in vec2 vUV;
        out vec4 FragColor;

        uniform sampler2D uSource;
        uniform vec3 uTexelSize;
        uniform int uHorizontal;

        void main() {
            const float weights[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
            vec3 result = texture(uSource, vUV).rgb * weights[0];

            vec2 axis = (uHorizontal == 1) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
            for (int i = 1; i < 5; ++i) {
                vec2 offset = axis * uTexelSize.xy * float(i);
                result += texture(uSource, vUV + offset).rgb * weights[i];
                result += texture(uSource, vUV - offset).rgb * weights[i];
            }
            FragColor = vec4(result, 1.0);
        }
    )";

    m_extractShader = std::make_unique<Shader>(vs, extractFs);
    m_blurShader = std::make_unique<Shader>(vs, blurFs);
}

bool BloomPass::ensure(int width, int height) {
    if (width <= 0 || height <= 0) return false;

    if (m_width == width && m_height == height && m_brightPass.isValid() && m_ping.isValid() && m_pong.isValid()) {
        return true;
    }

    m_width = width;
    m_height = height;

    std::vector<ColorAttachmentDesc> attachments;
    attachments.push_back(ColorAttachmentDesc{});

    const int bloomW = std::max(1, width / 2);
    const int bloomH = std::max(1, height / 2);

    if (!m_brightPass.create(bloomW, bloomH, attachments, DepthAttachmentMode::None)) return false;
    if (!m_ping.create(bloomW, bloomH, attachments, DepthAttachmentMode::None)) return false;
    if (!m_pong.create(bloomW, bloomH, attachments, DepthAttachmentMode::None)) return false;

    return true;
}

uint32_t BloomPass::execute(uint32_t hdrTexture, uint32_t screenVAO) {
    if (!m_brightPass.isValid() || !m_ping.isValid() || !m_pong.isValid()) {
        return hdrTexture;
    }

    const int bloomW = std::max(1, m_width / 2);
    const int bloomH = std::max(1, m_height / 2);

    glDisable(GL_DEPTH_TEST);

    glBindFramebuffer(GL_FRAMEBUFFER, m_brightPass.id());
    glViewport(0, 0, bloomW, bloomH);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    m_extractShader->bind();
    m_extractShader->setUniform("uSource", 0);
    m_extractShader->setUniform("uThreshold", 1.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);

    glBindVertexArray(screenVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    bool horizontal = true;
    uint32_t source = m_brightPass.colorTexture(0);
    constexpr int blurPasses = 6;

    for (int i = 0; i < blurPasses; ++i) {
        FrameBuffer& target = horizontal ? m_ping : m_pong;

        glBindFramebuffer(GL_FRAMEBUFFER, target.id());
        glViewport(0, 0, bloomW, bloomH);

        m_blurShader->bind();
        m_blurShader->setUniform("uSource", 0);
        m_blurShader->setUniform("uHorizontal", horizontal ? 1 : 0);
        m_blurShader->setUniform("uTexelSize", Vec3{1.0f / float(bloomW), 1.0f / float(bloomH), 0.0f});

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, source);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        source = target.colorTexture(0);
        horizontal = !horizontal;
    }

    glBindVertexArray(0);
    return source;
}

} // namespace rex::gfx
