#include "Fonts/Font.hpp"

using namespace ic;
using font::CFont;

FT_Library   CFont::m_Library;
gfx::CEffect CFont::m_FontRender;

bool CFont::Initialize()
{
    if(FT_Init_FreeType(&m_Library) != 0)       return false;
    if(!m_FontRender.Init(gfx::IC_GRAYSCALE))   return false;

    m_FontRender.Enable();
    m_FontRender.SetMatrix("mv", math::IDENTITY);
    m_FontRender.SetMatrix("proj", gfx::CWindow::GetProjectionMatrix());
    m_FontRender.Disable();

    return true;
}

bool CFont::LoadFromFile(const std::string& filename, const uint16_t size)
{
    if(FT_New_Face(m_Library, filename.c_str(), 0, &m_FontFace) != 0)
        return false;

    if(FT_Set_Char_Size(m_FontFace, size << 6, size << 6, 96, 96) != 0)
        return false;

    // Load all printable characters.
    for(size_t i = ' '; i < '~'; ++i)
    {
        FT_BBox  bbox;
        FT_Glyph glyph;

        uint32_t index = FT_Get_Char_Index(m_FontFace, i);
        if(index == 0) continue;

        FT_Load_Glyph(m_FontFace, index, FT_LOAD_RENDER);
        FT_Render_Glyph(m_FontFace->glyph, FT_RENDER_MODE_NORMAL);
        FT_Get_Glyph(m_FontFace->glyph, &glyph);
        FT_Glyph_Get_CBox(glyph, 3, &bbox);

        FT_GlyphSlot slot = m_FontFace->glyph;
        FT_Bitmap& bitmap = slot->bitmap;

        uint32_t w = bitmap.width;
        uint32_t h = bitmap.rows;

        asset::CTexture* pTexture = 
            asset::CAssetManager::Create<asset::CTexture>();

        // Our buffer has two channels, red and green. These
        // represent the gray scale value in the bitmap and the
        // alpha value for aliasing.
        unsigned char* data = new unsigned char[w * h];
        //memset(data, NULL, w * h *  sizeof(unsigned char));
        memcpy(data, bitmap.buffer, sizeof(unsigned char) * w * h);

        // Alignment for pixels needs to be at 1 byte.
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        pTexture->LoadFromRaw(GL_R8, GL_RED, w, h, data);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        delete[] data;

        FT_Done_Glyph(glyph);

        Glyph render_glyph;
        render_glyph.pTexture = pTexture;
        render_glyph.dim      = math::rect_t(w, h,
            slot->advance.x / 64,
            slot->metrics.horiBearingY / 64);

        mp_glyphTextures[i] = render_glyph;
    }

    FT_Done_Face(m_FontFace);

    return true;
}

math::rect_t CFont::RenderText(const std::string& text, const math::vector2_t& Pos)
{
    // VBO that will contain our text vertex data.
    gfx::CVertexBuffer VBO;

    // Track total text size.
    math::rect_t Size(Pos.x, Pos.y, 0, 0);

    if(!m_loaded || text.empty()) return Size;

    // Vertex buffer size, index buffer size.
    uint16_t vlen = text.length() << 2;
    uint16_t ilen = text.length() * ((1 << 2) + 2);

    // Create buffers and zero them.
    vertex2_t* verts = new vertex2_t[vlen];
    uint16_t*  inds  = new uint16_t[ilen];

    memset(inds, NULL,  sizeof(uint16_t)  * ilen);
    memset(verts, NULL, sizeof(vertex2_t) * vlen);

    // Track width and max height.
    int max_w = 0, max_h = 0;

    // Fill up buffers.
    int32_t last_w = Pos.x;
    for(size_t i = 0; i < vlen; i += 4)
    {
        math::rect_t Dim = mp_glyphTextures[text[i>>2]].dim;

        float w = last_w;
        float h = Dim.y;
        last_w  = w + /*Dim.x + */ Dim.w;

        verts[i].Position   = math::vector2_t(w, Pos.y - Dim.h);
        verts[i+1].Position = math::vector2_t(last_w, Pos.y - Dim.h);
        verts[i+2].Position = math::vector2_t(last_w, Pos.y - Dim.h + h);
        verts[i+3].Position = math::vector2_t(w, Pos.y - Dim.h + h);

        verts[i].TexCoord   = math::vector2_t(0, 0);
        verts[i+1].TexCoord = math::vector2_t(1, 0);
        verts[i+2].TexCoord = math::vector2_t(1, 1);
        verts[i+3].TexCoord = math::vector2_t(0, 1);

        for(size_t j = i; j < i + 4; ++j)
            verts[j].Color = m_Color;//color4f_t(1.f, 1.f, 1.f, 1.f);

        int x = (i >> 2) * 6;
        inds[x]   = i;
        inds[x+1] = i + 1;
        inds[x+2] = i + 3;
        inds[x+3] = i + 3;
        inds[x+4] = i + 2;
        inds[x+5] = i + 1;

        max_w += Dim.w;
        max_h  = math::max<int>(max_h, Dim.h + h);
    }

    // Tally up dimensions.
    Size.w = max_w;
    Size.h = max_h;

    // Enable font-rendering shader.
    m_FontRender.Enable();
    
    // Give data to GPU.
    VBO.Init();
    VBO.AddData(verts, vlen, inds, ilen);
    VBO.FinalizeBuffer();
    VBO.Bind();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Draw each character with its texture enabled.
    for(size_t i = 0; i < text.length(); ++i)
    {
        mp_glyphTextures[text[i]].pTexture->Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT,
            (void*)(sizeof(uint16_t) * i * 6));
    }

    glDisable(GL_BLEND);
    
    // Delete GPU buffers.
    VBO.Clear();

    // Unbind all the things.
    glBindTexture(GL_TEXTURE_2D, 0);    
    VBO.Unbind();
    m_FontRender.Disable();

    // Delete old buffers in RAM.
    delete[] verts;
    delete[] inds;
    
    return Size;
}

bool CFont::Resize(const uint32_t size)
{
    m_size = size;
    return this->LoadFromFile(m_filename, m_size);
}

void ic::font::CFont::SetFontColor(const color4f_t& Color)
{
    m_Color = Color;
}

void ic::font::CFont::SetFontColor(const float r, const float g, const float b)
{
    m_Color.r = r;
    m_Color.g = g;
    m_Color.b = b;
    m_Color.a = 1.f;
}
