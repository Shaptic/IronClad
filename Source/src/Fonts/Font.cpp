#include "Fonts/Font.hpp"

using namespace ic;
using font::CFont;

FT_Library CFont::m_Library;

gfx::CFrameBuffer* CFont::RenderText(const std::string& text)
{
    if(!m_loaded) return NULL;

    gfx::CFrameBuffer* pFbo = new gfx::CFrameBuffer;
    std::vector<uint32_t> textures;
    //uint32_t width, height;

    textures.resize(text.length());
    glGenTextures(text.length(), &textures[0]);

    for(size_t i = 0; i < text.length(); ++i)
    {
        glBindTexture(GL_TEXTURE_2D, textures[i]);

        uint32_t index = FT_Get_Char_Index(m_FontFace, text[i]);    
        FT_Load_Char(m_FontFace, index, FT_LOAD_DEFAULT);
        FT_Render_Glyph(m_FontFace->glyph, FT_RENDER_MODE_NORMAL);

        glTexImage2D(GL_TEXTURE_2D, 0, 
            m_FontFace->glyph->bitmap.pixel_mode,
            m_FontFace->glyph->metrics.width,
            m_FontFace->glyph->metrics.height, 0,
            m_FontFace->glyph->bitmap.pixel_mode,
            GL_UNSIGNED_BYTE,
            m_FontFace->glyph->bitmap.buffer);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    return pFbo;
}
