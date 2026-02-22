#include "PostProcessPipeline.h"

namespace rex::gfx {

PostProcessPipeline::PostProcessPipeline() {
    m_exposure.setManualExposure(m_settings.exposure);
}

PostProcessPipeline::~PostProcessPipeline() {
    if (m_fallbackBlackTex) {
        glDeleteTextures(1, &m_fallbackBlackTex);
        m_fallbackBlackTex = 0;
    }
}

void PostProcessPipeline::ensureFallbackTexture() {
    if (m_fallbackBlackTex) return;
    glGenTextures(1, &m_fallbackBlackTex);
    glBindTexture(GL_TEXTURE_2D, m_fallbackBlackTex);
    const float black[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1, 1, 0, GL_RGBA, GL_FLOAT, black);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void PostProcessPipeline::execute(uint32_t hdrTexture,
                                  uint32_t backbufferFBO,
                                  int width,
                                  int height,
                                  uint32_t screenVAO,
                                  float dt,
                                  float averageLuminance,
                                  uint32_t colorLUT) {
    if (width <= 0 || height <= 0) return;

    ensureFallbackTexture();

    m_exposure.setAutoExposure(m_settings.autoExposure);
    m_exposure.setManualExposure(m_settings.exposure);
    m_exposure.update(averageLuminance, dt);

    uint32_t bloomTex = m_fallbackBlackTex;
    if (m_settings.enableBloom) {
        m_bloom.ensure(width, height);
        bloomTex = m_bloom.execute(hdrTexture, screenVAO);
    }

    m_toneMapping.execute(hdrTexture,
                          bloomTex,
                          backbufferFBO,
                          width,
                          height,
                          screenVAO,
                          m_exposure.exposure(),
                          m_settings.bloomStrength,
                          colorLUT);
}

} // namespace rex::gfx
