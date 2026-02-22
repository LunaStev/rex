#pragma once

#include "../../Graphics/Shader.h"

#include <memory>

namespace rex::gfx {

class ToneMappingPass {
public:
    ToneMappingPass();

    void execute(uint32_t hdrTexture,
                 uint32_t bloomTexture,
                 uint32_t targetFBO,
                 int width,
                 int height,
                 uint32_t screenVAO,
                 float exposure,
                 float bloomStrength,
                 uint32_t colorLUT = 0);

private:
    std::unique_ptr<Shader> m_shader;
};

} // namespace rex::gfx
