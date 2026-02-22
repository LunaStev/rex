#include "RexUIFont.h"
#include "../Core/Logger.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <vector>
#include <algorithm>

namespace rex::ui {

RexUIFont::RexUIFont() = default;
RexUIFont::~RexUIFont() { clear(); }

void RexUIFont::clear() {
    if (m_texture) {
        glDeleteTextures(1, &m_texture);
        m_texture = 0;
    }
    m_glyphs.clear();
}

bool RexUIFont::load(const std::string& path, int pixelSize) {
    clear();

    FT_Library ft;
    if (FT_Init_FreeType(&ft) != 0) {
        Logger::error("FreeType init failed");
        return false;
    }

    FT_Face face;
    if (FT_New_Face(ft, path.c_str(), 0, &face) != 0) {
        Logger::error("FreeType load failed: {}", path);
        FT_Done_FreeType(ft);
        return false;
    }

    FT_Set_Pixel_Sizes(face, 0, pixelSize);
    m_lineHeight = (face->size->metrics.height >> 6);

    const int atlasW = 512;
    const int atlasH = 512;
    std::vector<unsigned char> atlas(atlasW * atlasH, 0);

    int penX = 0;
    int penY = 0;
    int rowH = 0;

    for (unsigned char c = 32; c < 127; ++c) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER) != 0)
            continue;

        FT_GlyphSlot g = face->glyph;
        if (penX + g->bitmap.width >= atlasW) {
            penX = 0;
            penY += rowH + 1;
            rowH = 0;
        }

        if (penY + g->bitmap.rows >= atlasH)
            break;

        for (int y = 0; y < (int)g->bitmap.rows; ++y) {
            for (int x = 0; x < (int)g->bitmap.width; ++x) {
                int dst = (penX + x) + (penY + y) * atlasW;
                atlas[dst] = g->bitmap.buffer[y * g->bitmap.pitch + x];
            }
        }

        Glyph glyph;
        glyph.size = {(float)g->bitmap.width, (float)g->bitmap.rows};
        glyph.bearing = {(float)g->bitmap_left, (float)g->bitmap_top};
        glyph.advance = (float)(g->advance.x >> 6);
        glyph.uv0 = {(float)penX / atlasW, (float)penY / atlasH};
        glyph.uv1 = {(float)(penX + g->bitmap.width) / atlasW, (float)(penY + g->bitmap.rows) / atlasH};
        m_glyphs[(char)c] = glyph;

        penX += g->bitmap.width + 1;
        rowH = std::max(rowH, (int)g->bitmap.rows);
    }

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlasW, atlasH, 0, GL_RED, GL_UNSIGNED_BYTE, atlas.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    m_texW = atlasW;
    m_texH = atlasH;

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    return true;
}

const Glyph* RexUIFont::getGlyph(char c) const {
    auto it = m_glyphs.find(c);
    if (it == m_glyphs.end())
        return nullptr;
    return &it->second;
}

} // namespace rex::ui
