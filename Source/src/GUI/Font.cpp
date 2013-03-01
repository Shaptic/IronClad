#include "GUI/Font.hpp"

using namespace ic;
using util::g_Log;
using gui::CFont;

FT_Library CFont::s_Library;
bool       CFont::s_initialized;

bool CFont::Initialize()
{
    return (s_initialized = (FT_Init_FreeType(&s_Library) == 0));
}

void CFont::DeInitialize()
{
    if(s_initialized) FT_Done_FreeType(s_Library);
}

CFont::CFont() : m_size(0), m_loaded(false)
{
    m_FontRender.Init(gfx::IC_GRAYSCALE);
    
    // Set the matrices in the rendering shader.
    m_FontRender.Enable();
    m_FontRender.SetMatrix("mv", math::IDENTITY);
    m_FontRender.SetMatrix("proj", gfx::CWindow::GetProjectionMatrix());
    m_FontRender.Disable();

    m_Cache.Init();
}

CFont::~CFont() {}

bool CFont::LoadFromFile(const std::string& filename, const uint16_t size)
{
    g_Log.Flush();
    g_Log << "[INFO] Loading font:      " << filename << "\n";
    g_Log.PrintLastLog();

    if(FT_New_Face(s_Library, filename.c_str(), 0, &m_FontFace) != 0)
        return false;

    // Set the size of the font face.
    // Since the function expects a size in 1/64 pixels, we multiply
    // by 64 (same as left-shifting 6 bits) before passing.
    // The 96 represents a 96-dpi font bitmap.
    if(FT_Set_Char_Size(m_FontFace, size << 6, size << 6, 96, 96) != 0)
        return false;

    // For logging :)
    std::stringstream ss;

    // Load all printable characters.
    // If you visit an ASCII table (like www.asciitable.com) you will see
    // that the only valid values for printing are the space character all
    // the way up to the tilde (~).
    for(size_t i = ' '; i <= '~'; ++i)
    {
        FT_Glyph glyph;

        // Locate the index of the character in the font face.
        uint32_t index = FT_Get_Char_Index(m_FontFace, i);
        if(index == 0) continue;

        // Load the glyph into the font face.
        FT_Load_Glyph(m_FontFace, index, FT_LOAD_RENDER);

        // Render the glyph as a mono-chrome bitmap.
        FT_Render_Glyph(m_FontFace->glyph, FT_RENDER_MODE_NORMAL);

        // Put the glyph in the glyph slot into an actual glyph struct.
        FT_Get_Glyph(m_FontFace->glyph, &glyph);

        FT_GlyphSlot slot = m_FontFace->glyph;
        FT_Bitmap& bitmap = slot->bitmap;

        // Bitmap dimensions
        uint32_t w = bitmap.width;
        uint32_t h = bitmap.rows;

        // Create the OpenGL bitmap texture handle.
        asset::CTexture* pTexture = 
            asset::CAssetManager::Create<asset::CTexture>();

        // Log filename as font_name:char
        ss << filename << ":" << (char)i;
        pTexture->SetFilename(ss.str());
        ss.str(std::string());

        // We need to copy the data over to a new buffer in order
        // to properly pass it to the GPU.
        unsigned char* data = new unsigned char[w * h];
        memset(data, NULL, w * h *  sizeof(unsigned char));
        memcpy(data, bitmap.buffer, sizeof(unsigned char) * w * h);

        // Alignment for pixels needs to be at 1 byte.
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        pTexture->LoadFromRaw(GL_R8, GL_RED, w, h, data);

        // Restore default alignment value. I haven't actually tested this
        // part so it may or may not actually be the default.
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

        // Delete bitmap buffers        
        delete[] data;
        FT_Done_Glyph(glyph);

        Glyph render_glyph;
        render_glyph.pTexture = pTexture;
        render_glyph.dim      = math::rect_t(w, h,
            slot->advance.x / 64,
            slot->metrics.horiBearingY / 64);

        mp_glyphTextures[i] = render_glyph;
    }

    m_loaded    = true;
    m_filename  = filename;
    return (FT_Done_Face(m_FontFace) == 0);
}

math::rect_t CFont::RenderText(const std::string& text,
                               const math::vector2_t& Pos)
{
    // VBO that will contain our text vertex data.
    gfx::CVertexBuffer VBO;

    // Track total text size.
    math::rect_t Size(Pos.x, Pos.y, 0, 0);

    if(!m_loaded || text.empty()) return Size;

    // Vertex buffer size, index buffer size.
    uint16_t vlen = text.length() << 2;
    uint16_t ilen = text.length() * 6;

    // Create buffers and zero them.
    vertex2_t* verts = new vertex2_t[vlen];
    uint16_t*  inds  = new uint16_t [ilen];

    memset(inds,  NULL, sizeof(uint16_t)  * ilen);
    memset(verts, NULL, sizeof(vertex2_t) * vlen);

    // Track width and max height.
    int max_w = 0, max_h = 0;

    // The x-position to start the next character at.
    int32_t last_w = Pos.x;

    // Fill up buffers. Each character needs 4 vertices, so
    // we increment by 4 each iteration then compensate for
    // that throughout the loop.
    for(size_t i = 0; i < vlen; i += 4)
    {
        // Retrieve dimensions from the dictionary.
        // Since we're doing i += 4, the index in the text string
        // would be text[i / 4].
        rect_t Dim = mp_glyphTextures[text[i >> 2]].dim;

        float w = last_w;
        float h = Dim.y;
        last_w  = w + Dim.w;    // Increase until next char by the
                                // bitmap's horizontal advance value.

        // [i]      : top left
        // [i + 1]  : top right
        // [i + 2]  : bottom right
        // [i + 3]  : bottom left
        verts[i].Position   = math::vector2_t(w,        Pos.y - Dim.h);
        verts[i+1].Position = math::vector2_t(last_w,   Pos.y - Dim.h);
        verts[i+2].Position = math::vector2_t(last_w,   Pos.y - Dim.h + h);
        verts[i+3].Position = math::vector2_t(w,        Pos.y - Dim.h + h);

        // Load up the bitmap texture coordinates moving counter-clockwise
        // from the origin.
        verts[i].TexCoord   = math::vector2_t(0, 0);
        verts[i+1].TexCoord = math::vector2_t(1, 0);
        verts[i+2].TexCoord = math::vector2_t(1, 1);
        verts[i+3].TexCoord = math::vector2_t(0, 1);

        // The vertices all use the font color.
        // See CFont::SetColor()
        for(size_t j = i; j < i + 4; ++j)
            verts[j].Color = m_Color;

        // The index in the buffer that we need is i / 4 * 6, since 
        // i / 4 gives us the current character, and each character
        // needs 6 indices.
        int x = (i >> 2) * 6;

        // Assign the values.
        inds[x]   = i;
        inds[x+1] = i + 1;
        inds[x+2] = i + 3;
        inds[x+3] = i + 3;
        inds[x+4] = i + 2;
        inds[x+5] = i + 1;

        // Keep track of the overall dimensions.
        max_w += Dim.w;
        max_h  = ic::math::max<int>(max_h, Dim.h + h);
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

    m_last_text = text;
    
    return Size;
}

math::rect_t CFont::RenderCached()
{
    // Enable font-rendering shader.
    m_FontRender.Enable();

    m_Cache.Bind();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Draw each character with its texture enabled.
    for(size_t i = 0; i < m_last_text.length(); ++i)
    {
        mp_glyphTextures[m_last_text[i]].pTexture->Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT,
                      (void*)(sizeof(uint16_t) * i * 6));
    }

    glDisable(GL_BLEND);

    // Unbind all the things.
    glBindTexture(GL_TEXTURE_2D, 0);    
    m_Cache.Unbind();
    m_FontRender.Disable();

    return m_CacheSize;
}

math::rect_t CFont::CacheText(const std::string& text,
                              const math::vector2_t& Pos)
{
    m_Cache.Clear();

    if(!m_loaded || text.empty())
        return (m_CacheSize = math::rect_t(Pos.x, Pos.y, 0, 0));

    // Vertex buffer size, index buffer size.
    uint16_t vlen = text.length() << 2;
    uint16_t ilen = text.length() * 6;

    // Create buffers and zero them.
    vertex2_t* verts = new vertex2_t[vlen];
    uint16_t*  inds  = new uint16_t [ilen];

    memset(inds,  NULL, sizeof(uint16_t)  * ilen);
    memset(verts, NULL, sizeof(vertex2_t) * vlen);

    // Track width and max height.
    int max_w = 0, max_h = 0;

    // The x-position to start the next character at.
    int32_t last_w = Pos.x;

    // Fill up buffers. Each character needs 4 vertices, so
    // we increment by 4 each iteration then compensate for
    // that throughout the loop.
    for(size_t i = 0; i < vlen; i += 4)
    {
        // Retrieve dimensions from the dictionary.
        // Since we're doing i += 4, the index in the text string
        // would be text[i / 4].
        rect_t Dim = mp_glyphTextures[text[i >> 2]].dim;

        float w = last_w;
        float h = Dim.y;
        last_w  = w + Dim.w;    // Increase until next char by the
        // bitmap's horizontal advance value.

        // [i]      : top left
        // [i + 1]  : top right
        // [i + 2]  : bottom right
        // [i + 3]  : bottom left
        verts[i].Position   = math::vector2_t(w,        Pos.y - Dim.h);
        verts[i+1].Position = math::vector2_t(last_w,   Pos.y - Dim.h);
        verts[i+2].Position = math::vector2_t(last_w,   Pos.y - Dim.h + h);
        verts[i+3].Position = math::vector2_t(w,        Pos.y - Dim.h + h);

        // Load up the bitmap texture coordinates moving counter-clockwise
        // from the origin.
        verts[i].TexCoord   = math::vector2_t(0, 0);
        verts[i+1].TexCoord = math::vector2_t(1, 0);
        verts[i+2].TexCoord = math::vector2_t(1, 1);
        verts[i+3].TexCoord = math::vector2_t(0, 1);

        // The vertices all use the font color.
        // See CFont::SetColor()
        for(size_t j = i; j < i + 4; ++j)
            verts[j].Color = m_Color;

        // The index in the buffer that we need is i / 4 * 6, since 
        // i / 4 gives us the current character, and each character
        // needs 6 indices.
        int x = (i >> 2) * 6;

        // Assign the values.
        inds[x]   = i;
        inds[x+1] = i + 1;
        inds[x+2] = i + 3;
        inds[x+3] = i + 3;
        inds[x+4] = i + 2;
        inds[x+5] = i + 1;

        // Keep track of the overall dimensions.
        max_w += Dim.w;
        max_h  = ic::math::max<int>(max_h, Dim.h + h);
    }

    // Tally up dimensions.
    m_CacheSize.x = Pos.x;
    m_CacheSize.y = Pos.y;
    m_CacheSize.w = max_w;
    m_CacheSize.h = max_h;

    // Give data to GPU.
    if(!m_Cache.Init()) printf("FUCK.\n");
    m_Cache.AddData(verts, vlen, inds, ilen);
    m_Cache.FinalizeBuffer();

    // Delete old buffers in RAM.
    delete[] verts;
    delete[] inds;

    m_last_text = text;

    return m_CacheSize;
}

bool CFont::Resize(const uint32_t size)
{
    m_size = size;
    return this->LoadFromFile(m_filename, m_size);
}

void ic::gui::CFont::SetFontColor(const color4f_t& Color)
{
    m_Color = Color;
}

void ic::gui::CFont::SetFontColor(const float r, const float g, const float b)
{
    m_Color.r = r;
    m_Color.g = g;
    m_Color.b = b;
    m_Color.a = 1.f;
}

uint32_t CFont::GetTextWidth(const char* text)
{
    if(text == NULL) return 0;

    uint32_t w = 0;
    uint32_t l = strlen(text);

    for(size_t i = 0; i < l; ++i)
    {
        w += mp_glyphTextures[text[i]].dim.w;
    }

    return w;
}

uint32_t CFont::GetTextHeight(const char* text)
{
    if(text == NULL) return 0;

    uint32_t h = 0;
    uint32_t l = strlen(text);

    for(size_t i = 0; i < l; ++i)
    {
        h = math::max<int>(mp_glyphTextures[text[i]].dim.y, h);
    }

    return h;
}