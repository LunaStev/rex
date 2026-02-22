#pragma once
#include "RexUI.h"
#include "RexUIFont.h"
#include "../Graphics/Shader.h"
#include "../Graphics/GLInternal.h"
#include <vector>

namespace rex::ui {

class RexUIRenderer {
public:
    RexUIRenderer();
    ~RexUIRenderer();

    void render(const UIContext& ctx, int viewportWidth, int viewportHeight);

private:
    void initGL();
    void buildVertices(const UIContext& ctx);
    void buildTextVertices(const UIContext& ctx);

    std::unique_ptr<Shader> m_shader;
    std::unique_ptr<Shader> m_textShader;
    uint32_t m_vao = 0;
    uint32_t m_vbo = 0;
    std::vector<float> m_vertices;

    uint32_t m_textVAO = 0;
    uint32_t m_textVBO = 0;
    std::vector<float> m_textVertices;
    RexUIFont m_font;
};

} // namespace rex::ui
