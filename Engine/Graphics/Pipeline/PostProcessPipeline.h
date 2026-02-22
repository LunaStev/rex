#pragma once

#include "../HDR/ExposureController.h"
#include "../PostProcess/BloomPass.h"
#include "../PostProcess/ToneMappingPass.h"

namespace rex::gfx {

struct PostProcessSettings {
    bool enableBloom = true;
    bool autoExposure = false;
    float exposure = 1.2f;
    float bloomStrength = 0.08f;
};

class PostProcessPipeline {
public:
    PostProcessPipeline();
    ~PostProcessPipeline();

    PostProcessSettings& settings() { return m_settings; }
    const PostProcessSettings& settings() const { return m_settings; }

    void execute(uint32_t hdrTexture,
                 uint32_t backbufferFBO,
                 int width,
                 int height,
                 uint32_t screenVAO,
                 float dt,
                 float averageLuminance = 1.0f,
                 uint32_t colorLUT = 0);

private:
    PostProcessSettings m_settings;
    ExposureController m_exposure;
    BloomPass m_bloom;
    ToneMappingPass m_toneMapping;

    uint32_t m_fallbackBlackTex = 0;
    void ensureFallbackTexture();
};

} // namespace rex::gfx
