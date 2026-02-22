#include "RexUIRendererGL.h"

#include <algorithm>
#include <fstream>
#include <vector>

#include "../../../../Core/Logger.h"

namespace rex::ui::renderer::opengl {

namespace {
struct RectVertex {
    float x;
    float y;
    float r;
    float g;
    float b;
    float a;
};

struct TextVertex {
    float x;
    float y;
    float u;
    float v;
    float r;
    float g;
    float b;
    float a;
};

inline void pushRectVertices(std::vector<RectVertex>& out, const core::Rect& rect, const core::Color& c) {
    const RectVertex v0{rect.x, rect.y, c.r, c.g, c.b, c.a};
    const RectVertex v1{rect.x + rect.w, rect.y, c.r, c.g, c.b, c.a};
    const RectVertex v2{rect.x + rect.w, rect.y + rect.h, c.r, c.g, c.b, c.a};
    const RectVertex v3{rect.x, rect.y + rect.h, c.r, c.g, c.b, c.a};

    out.push_back(v0);
    out.push_back(v1);
    out.push_back(v2);
    out.push_back(v0);
    out.push_back(v2);
    out.push_back(v3);
}

inline float maxf(float a, float b) {
    return a > b ? a : b;
}

inline float minf(float a, float b) {
    return a < b ? a : b;
}
} // namespace

RexUIRendererGL::RexUIRendererGL(IRenderDevice* device)
    : device_(device) {}

RexUIRendererGL::~RexUIRendererGL() {
    destroyGpuResources();
}

bool RexUIRendererGL::ensureInitialized() {
    if (initialized_) return true;

    const std::string rectVs = R"(
        #version 330 core
        layout(location = 0) in vec2 aPos;
        layout(location = 1) in vec4 aColor;
        uniform vec2 uViewport;
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

    const std::string rectFs = R"(
        #version 330 core
        in vec4 vColor;
        out vec4 FragColor;
        void main() {
            FragColor = vColor;
        }
    )";

    const std::string textVs = R"(
        #version 330 core
        layout(location = 0) in vec2 aPos;
        layout(location = 1) in vec2 aUv;
        layout(location = 2) in vec4 aColor;
        uniform vec2 uViewport;
        out vec2 vUv;
        out vec4 vColor;
        void main() {
            vec2 ndc = vec2(
                (aPos.x / uViewport.x) * 2.0 - 1.0,
                1.0 - (aPos.y / uViewport.y) * 2.0
            );
            vUv = aUv;
            vColor = aColor;
            gl_Position = vec4(ndc, 0.0, 1.0);
        }
    )";

    const std::string textFs = R"(
        #version 330 core
        in vec2 vUv;
        in vec4 vColor;
        uniform sampler2D uFont;
        out vec4 FragColor;
        void main() {
            float alpha = texture(uFont, vUv).r;
            FragColor = vec4(vColor.rgb, vColor.a * alpha);
        }
    )";

    rectShader_ = std::make_unique<::rex::Shader>(rectVs, rectFs);
    textShader_ = std::make_unique<::rex::Shader>(textVs, textFs);

    glGenVertexArrays(1, &rectVao_);
    glGenBuffers(1, &rectVbo_);
    glBindVertexArray(rectVao_);
    glBindBuffer(GL_ARRAY_BUFFER, rectVbo_);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(RectVertex), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(RectVertex), reinterpret_cast<void*>(sizeof(float) * 2));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glGenVertexArrays(1, &textVao_);
    glGenBuffers(1, &textVbo_);
    glBindVertexArray(textVao_);
    glBindBuffer(GL_ARRAY_BUFFER, textVbo_);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), reinterpret_cast<void*>(sizeof(float) * 2));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(TextVertex), reinterpret_cast<void*>(sizeof(float) * 4));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    const char* fontCandidates[] = {
        "Engine/UI/Fonts/Roboto-Regular.ttf",
        "Engine/UI/Fonts/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/dejavu/DejaVuSans.ttf"
    };

    for (const char* path : fontCandidates) {
        std::ifstream file(path, std::ios::binary);
        if (!file.good()) continue;
        if (font_.load(path, 16)) {
            fontReady_ = true;
            Logger::info("RexUI GL font loaded: {}", path);
            break;
        }
    }

    if (!fontReady_) {
        Logger::warn("RexUI GL text disabled: no font loaded");
    }

    initialized_ = true;
    return true;
}

void RexUIRendererGL::destroyGpuResources() {
    if (rectVbo_ != 0) {
        glDeleteBuffers(1, &rectVbo_);
        rectVbo_ = 0;
    }
    if (rectVao_ != 0) {
        glDeleteVertexArrays(1, &rectVao_);
        rectVao_ = 0;
    }
    if (textVbo_ != 0) {
        glDeleteBuffers(1, &textVbo_);
        textVbo_ = 0;
    }
    if (textVao_ != 0) {
        glDeleteVertexArrays(1, &textVao_);
        textVao_ = 0;
    }
}

bool RexUIRendererGL::beginFrame(const RenderFrameContext& context) {
    if (!ensureInitialized()) return false;
    frame_ = context;
    clipStack_.clear();
    applyClipState();
    return true;
}

core::Rect RexUIRendererGL::intersectClips(const core::Rect& a, const core::Rect& b) const {
    const float x0 = maxf(a.x, b.x);
    const float y0 = maxf(a.y, b.y);
    const float x1 = minf(a.x + a.w, b.x + b.w);
    const float y1 = minf(a.y + a.h, b.y + b.h);
    return {
        x0,
        y0,
        maxf(0.0f, x1 - x0),
        maxf(0.0f, y1 - y0)
    };
}

void RexUIRendererGL::applyClipState() {
    if (clipStack_.empty()) {
        glDisable(GL_SCISSOR_TEST);
        return;
    }

    const auto& clip = clipStack_.back();
    const int vpH = static_cast<int>(frame_.viewportHeight);
    const int x = std::max(0, static_cast<int>(clip.x));
    const int y = std::max(0, vpH - static_cast<int>(clip.y + clip.h));
    const int w = std::max(0, static_cast<int>(clip.w));
    const int h = std::max(0, static_cast<int>(clip.h));

    glEnable(GL_SCISSOR_TEST);
    glScissor(x, y, w, h);
}

void RexUIRendererGL::submitRect(const core::Rect& rect, const core::Color& color) {
    std::vector<RectVertex> verts;
    verts.reserve(6);
    pushRectVertices(verts, rect, color);

    rectShader_->bind();
    rectShader_->setUniform("uViewport", Vec3{
        static_cast<float>(frame_.viewportWidth),
        static_cast<float>(frame_.viewportHeight),
        0.0f
    });
    glBindVertexArray(rectVao_);
    glBindBuffer(GL_ARRAY_BUFFER, rectVbo_);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(verts.size() * sizeof(RectVertex)), verts.data(), GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(verts.size()));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void RexUIRendererGL::submitBorder(const core::Rect& rect, const core::Color& color, float thickness) {
    const float t = std::max(1.0f, thickness);
    submitRect({rect.x, rect.y, rect.w, t}, color);
    submitRect({rect.x, rect.y + rect.h - t, rect.w, t}, color);
    submitRect({rect.x, rect.y, t, rect.h}, color);
    submitRect({rect.x + rect.w - t, rect.y, t, rect.h}, color);
}

void RexUIRendererGL::submitText(const core::Rect& rect, const std::string& text, const core::Color& color) {
    if (!fontReady_ || text.empty()) return;

    std::vector<TextVertex> verts;
    verts.reserve(text.size() * 6);

    float x = rect.x + 6.0f;
    float baselineY = rect.y + std::max(18.0f, rect.h - 5.0f);

    for (const char ch : text) {
        if (ch == '\n') {
            x = rect.x + 6.0f;
            baselineY += static_cast<float>(font_.lineHeight());
            continue;
        }

        const auto* g = font_.getGlyph(ch);
        if (!g) continue;

        const float xpos = x + g->bearing.x;
        const float ypos = baselineY - g->bearing.y;
        const float w = g->size.x;
        const float h = g->size.y;

        const TextVertex v0{xpos, ypos, g->uv0.x, g->uv0.y, color.r, color.g, color.b, color.a};
        const TextVertex v1{xpos + w, ypos, g->uv1.x, g->uv0.y, color.r, color.g, color.b, color.a};
        const TextVertex v2{xpos + w, ypos + h, g->uv1.x, g->uv1.y, color.r, color.g, color.b, color.a};
        const TextVertex v3{xpos, ypos + h, g->uv0.x, g->uv1.y, color.r, color.g, color.b, color.a};

        verts.push_back(v0);
        verts.push_back(v1);
        verts.push_back(v2);
        verts.push_back(v0);
        verts.push_back(v2);
        verts.push_back(v3);

        x += g->advance;
    }

    if (verts.empty()) return;

    textShader_->bind();
    textShader_->setUniform("uViewport", Vec3{
        static_cast<float>(frame_.viewportWidth),
        static_cast<float>(frame_.viewportHeight),
        0.0f
    });
    textShader_->setUniform("uFont", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, font_.textureId());
    glBindVertexArray(textVao_);
    glBindBuffer(GL_ARRAY_BUFFER, textVbo_);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(verts.size() * sizeof(TextVertex)), verts.data(), GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(verts.size()));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

bool RexUIRendererGL::submit(const runtime::render::DrawList& drawList) {
    if (!initialized_) return false;

    (void)device_;
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (const auto& cmd : drawList) {
        switch (cmd.type) {
            case runtime::render::DrawCommandType::Rect:
                submitRect(cmd.rect, cmd.color);
                break;
            case runtime::render::DrawCommandType::Border:
                submitBorder(cmd.rect, cmd.color, cmd.thickness);
                break;
            case runtime::render::DrawCommandType::Text:
                submitText(cmd.rect, cmd.text, cmd.color);
                break;
            case runtime::render::DrawCommandType::Image:
                // Placeholder path until image atlas integration lands.
                submitRect(cmd.rect, cmd.color);
                break;
            case runtime::render::DrawCommandType::ClipPush: {
                core::Rect next = cmd.rect;
                if (!clipStack_.empty()) {
                    next = intersectClips(clipStack_.back(), cmd.rect);
                }
                clipStack_.push_back(next);
                applyClipState();
                break;
            }
            case runtime::render::DrawCommandType::ClipPop:
                if (!clipStack_.empty()) {
                    clipStack_.pop_back();
                }
                applyClipState();
                break;
        }
    }

    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    return true;
}

bool RexUIRendererGL::endFrame() {
    return true;
}

} // namespace rex::ui::renderer::opengl
