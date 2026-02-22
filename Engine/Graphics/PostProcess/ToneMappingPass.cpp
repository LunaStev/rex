#include "ToneMappingPass.h"

namespace rex::gfx {

ToneMappingPass::ToneMappingPass() {
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

    const std::string fs = R"(
        #version 450 core
        in vec2 vUV;
        out vec4 FragColor;

        uniform sampler2D uHDR;
        uniform sampler2D uBloom;
        uniform sampler2D uColorLUT;
        uniform int uUseLUT;

        uniform float uExposure;
        uniform float uBloomStrength;

        vec3 ACESFilm(vec3 x) {
            const float a = 2.51;
            const float b = 0.03;
            const float c = 2.43;
            const float d = 0.59;
            const float e = 0.14;
            return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
        }

        void main() {
            vec3 hdr = texture(uHDR, vUV).rgb;
            vec3 bloom = texture(uBloom, vUV).rgb;
            vec3 color = hdr + bloom * uBloomStrength;

            color = vec3(1.0) - exp(-color * max(0.0001, uExposure));
            color = ACESFilm(color);

            if (uUseLUT == 1) {
                color = texture(uColorLUT, color.xy).rgb;
            }

            color = pow(color, vec3(1.0 / 2.2));
            FragColor = vec4(color, 1.0);
        }
    )";

    m_shader = std::make_unique<Shader>(vs, fs);
}

void ToneMappingPass::execute(uint32_t hdrTexture,
                              uint32_t bloomTexture,
                              uint32_t targetFBO,
                              int width,
                              int height,
                              uint32_t screenVAO,
                              float exposure,
                              float bloomStrength,
                              uint32_t colorLUT) {
    glBindFramebuffer(GL_FRAMEBUFFER, targetFBO);
    glViewport(0, 0, width, height);
    glDisable(GL_DEPTH_TEST);

    m_shader->bind();
    m_shader->setUniform("uExposure", exposure);
    m_shader->setUniform("uBloomStrength", bloomStrength);
    m_shader->setUniform("uHDR", 0);
    m_shader->setUniform("uBloom", 1);
    m_shader->setUniform("uColorLUT", 2);
    m_shader->setUniform("uUseLUT", colorLUT ? 1 : 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, bloomTexture);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, colorLUT ? colorLUT : bloomTexture);

    glBindVertexArray(screenVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

} // namespace rex::gfx
