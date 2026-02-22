#include "RexUIRenderer.h"
#include "../Core/Logger.h"
#include <fstream>

namespace rex::ui {

RexUIRenderer::RexUIRenderer() {
    initGL();
    const char* candidates[] = {
        "Engine/UI/Fonts/Roboto-Regular.ttf",
        "Engine/UI/Fonts/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/dejavu/DejaVuSans.ttf"
    };
    bool loaded = false;
    for (const char* p : candidates) {
        std::ifstream f(p, std::ios::binary);
        if (!f.good())
            continue;
        if (m_font.load(p, 16)) {
            loaded = true;
            Logger::info("RexUI font loaded: {}", p);
            break;
        }
    }
    if (!loaded) {
        Logger::error("RexUI font load failed. Add a TTF at Engine/UI/Fonts/Roboto-Regular.ttf");
    }
}

RexUIRenderer::~RexUIRenderer() {
    if (m_vbo) glDeleteBuffers(1, &m_vbo);
    if (m_vao) glDeleteVertexArrays(1, &m_vao);
    if (m_textVBO) glDeleteBuffers(1, &m_textVBO);
    if (m_textVAO) glDeleteVertexArrays(1, &m_textVAO);
}

void RexUIRenderer::initGL() {
    std::string vs = R"(
        #version 450 core
        layout(location = 0) in vec2 aPos;
        layout(location = 1) in vec4 aColor;
        uniform vec3 uViewport;
        out vec4 vColor;
        void main() {
            vec2 ndc = vec2(
                (aPos.x / uViewport.x) * 2.0 - 1.0,
                1.0 - (aPos.y / uViewport.y) * 2.0
            );
            vColor = aColor;
            gl_Position = vec4(ndc, 0.0, 1.0);
        }
    )";

    std::string fs = R"(
        #version 450 core
        in vec4 vColor;
        out vec4 FragColor;
        void main() {
            FragColor = vColor;
        }
    )";

    m_shader = std::make_unique<Shader>(vs, fs);

    std::string tvs = R"(
        #version 450 core
        layout(location = 0) in vec2 aPos;
        layout(location = 1) in vec2 aUV;
        layout(location = 2) in vec4 aColor;
        uniform vec3 uViewport;
        out vec2 vUV;
        out vec4 vColor;
        void main() {
            vec2 ndc = vec2(
                (aPos.x / uViewport.x) * 2.0 - 1.0,
                1.0 - (aPos.y / uViewport.y) * 2.0
            );
            vUV = aUV;
            vColor = aColor;
            gl_Position = vec4(ndc, 0.0, 1.0);
        }
    )";

    std::string tfs = R"(
        #version 450 core
        in vec2 vUV;
        in vec4 vColor;
        uniform sampler2D uFont;
        out vec4 FragColor;
        void main() {
            float a = texture(uFont, vUV).r;
            FragColor = vec4(vColor.rgb, vColor.a * a);
        }
    )";

    m_textShader = std::make_unique<Shader>(tvs, tfs);

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glGenVertexArrays(1, &m_textVAO);
    glGenBuffers(1, &m_textVBO);
    glBindVertexArray(m_textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_textVBO);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(4 * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

static inline void pushVertex(std::vector<float>& v, float x, float y, const UIColor& c) {
    v.push_back(x);
    v.push_back(y);
    v.push_back(c.r);
    v.push_back(c.g);
    v.push_back(c.b);
    v.push_back(c.a);
}

void RexUIRenderer::buildVertices(const UIContext& ctx) {
    m_vertices.clear();
    const auto& list = ctx.drawList();
    m_vertices.reserve(list.size() * 6 * 6);

    for (const auto& cmd : list) {
        if (cmd.type != UIDrawCommand::Type::Rect)
            continue;

        float x = cmd.rect.x;
        float y = cmd.rect.y;
        float w = cmd.rect.w;
        float h = cmd.rect.h;
        UIColor c = cmd.color;

        // Two triangles
        pushVertex(m_vertices, x,     y,     c);
        pushVertex(m_vertices, x + w, y,     c);
        pushVertex(m_vertices, x + w, y + h, c);

        pushVertex(m_vertices, x,     y,     c);
        pushVertex(m_vertices, x + w, y + h, c);
        pushVertex(m_vertices, x,     y + h, c);
    }
}

static inline void pushTextVertex(std::vector<float>& v, float x, float y, float u, float v0, const UIColor& c) {
    v.push_back(x);
    v.push_back(y);
    v.push_back(u);
    v.push_back(v0);
    v.push_back(c.r);
    v.push_back(c.g);
    v.push_back(c.b);
    v.push_back(c.a);
}

void RexUIRenderer::buildTextVertices(const UIContext& ctx) {
    m_textVertices.clear();
    const auto& list = ctx.drawList();
    for (const auto& cmd : list) {
        if (cmd.type != UIDrawCommand::Type::Text)
            continue;
        float x = cmd.rect.x + 4.0f;
        float y = cmd.rect.y + cmd.rect.h - 4.0f;
        UIColor c = cmd.color;

        for (char ch : cmd.text) {
            const Glyph* g = m_font.getGlyph(ch);
            if (!g) continue;

            float xpos = x + g->bearing.x;
            float ypos = y - g->bearing.y;
            float w = g->size.x;
            float h = g->size.y;

            float u0 = g->uv0.x;
            float v0 = g->uv0.y;
            float u1 = g->uv1.x;
            float v1 = g->uv1.y;

            pushTextVertex(m_textVertices, xpos,     ypos,     u0, v0, c);
            pushTextVertex(m_textVertices, xpos + w, ypos,     u1, v0, c);
            pushTextVertex(m_textVertices, xpos + w, ypos + h, u1, v1, c);

            pushTextVertex(m_textVertices, xpos,     ypos,     u0, v0, c);
            pushTextVertex(m_textVertices, xpos + w, ypos + h, u1, v1, c);
            pushTextVertex(m_textVertices, xpos,     ypos + h, u0, v1, c);

            x += g->advance;
        }
    }
}

void RexUIRenderer::render(const UIContext& ctx, int viewportWidth, int viewportHeight) {
    if (!m_shader)
        return;

    buildVertices(ctx);
    buildTextVertices(ctx);
    if (m_vertices.empty() && m_textVertices.empty())
        return;

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_shader->bind();
    m_shader->setUniform("uViewport", Vec3{(float)viewportWidth, (float)viewportHeight, 0});

    if (!m_vertices.empty()) {
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), m_vertices.data(), GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(m_vertices.size() / 6));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    if (!m_textVertices.empty() && m_textShader && m_font.textureId()) {
        m_textShader->bind();
        m_textShader->setUniform("uViewport", Vec3{(float)viewportWidth, (float)viewportHeight, 0});
        m_textShader->setUniform("uFont", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_font.textureId());

        glBindVertexArray(m_textVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_textVBO);
        glBufferData(GL_ARRAY_BUFFER, m_textVertices.size() * sizeof(float), m_textVertices.data(), GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(m_textVertices.size() / 8));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

} // namespace rex::ui
