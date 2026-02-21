#pragma once
#include "../Core/Scene.h"
#include "../Core/Components.h"
#include "../Graphics/Shader.h"
#include <functional>

namespace rex {

class Renderer {
public:
    Renderer();
    ~Renderer();

    void render(Scene& scene, const Camera& camera, const Mat4& viewMatrix, const Vec3& viewPos,
                int targetWidth, int targetHeight, uint32_t backbufferFBO,
                const std::function<void(const Mat4&, const Mat4&)>& extraDraw = {});

private:
    void ensureHDRResources(int w, int h);
    void initScreenTriangle();

    std::unique_ptr<Shader> m_pbrShader;
    std::unique_ptr<Shader> m_tonemapShader;

    uint32_t m_hdrFBO = 0;
    uint32_t m_hdrColor = 0;
    uint32_t m_hdrDepth = 0;
    int m_hdrWidth = 0;
    int m_hdrHeight = 0;

    uint32_t m_screenVAO = 0;
    uint32_t m_screenVBO = 0;
};

}
