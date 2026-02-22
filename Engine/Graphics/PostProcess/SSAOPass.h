#pragma once

#include "../../Graphics/Shader.h"
#include "../Core/FrameBuffer.h"

#include <memory>
#include <vector>

namespace rex::gfx {

class SSAOPass {
public:
    SSAOPass();
    ~SSAOPass();

    bool ensure(int width, int height);
    uint32_t execute(uint32_t gPosition,
                     uint32_t gNormal,
                     const Mat4& projection,
                     uint32_t screenVAO,
                     int width,
                     int height);

private:
    void buildKernelAndNoise();

    FrameBuffer m_ssaoBuffer;
    FrameBuffer m_blurBuffer;

    std::unique_ptr<Shader> m_ssaoShader;
    std::unique_ptr<Shader> m_blurShader;

    uint32_t m_noiseTex = 0;
    std::vector<Vec3> m_kernel;

    int m_width = 0;
    int m_height = 0;
};

} // namespace rex::gfx
