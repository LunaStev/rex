#pragma once
#include "../Graphics/GLInternal.h"
#include "../Core/RexMath.h"
#include <string>
#include <unordered_map>

namespace rex::ui {

struct Glyph {
    Vec2 size;
    Vec2 bearing;
    float advance = 0;
    Vec2 uv0;
    Vec2 uv1;
};

class RexUIFont {
public:
    RexUIFont();
    ~RexUIFont();

    bool load(const std::string& path, int pixelSize);
    const Glyph* getGlyph(char c) const;
    uint32_t textureId() const { return m_texture; }
    int lineHeight() const { return m_lineHeight; }

private:
    void clear();
    std::unordered_map<char, Glyph> m_glyphs;
    uint32_t m_texture = 0;
    int m_texW = 0;
    int m_texH = 0;
    int m_lineHeight = 0;
};

} // namespace rex::ui
