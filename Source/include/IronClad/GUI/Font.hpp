/**
 * @file
 *  Fonts/Font.hpp - A wrapper for an instance of a .ttf font using the FreeType 2 API.
 *
 * @author      George Kudrayvtsev (halcyon)
 * @version     1.1.2
 * @copyright   Apache License v2.0
 *  Licensed under the Apache License, Version 2.0 (the "License").         \n
 *  You may not use this file except in compliance with the License.        \n
 *  You may obtain a copy of the License at:
 *  http://www.apache.org/licenses/LICENSE-2.0                              \n
 *  Unless required by applicable law or agreed to in writing, software     \n
 *  distributed under the License is distributed on an "AS IS" BASIS,       \n
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\n
 *  See the License for the specific language governing permissions and     \n
 *  limitations under the License.
 *
 * @addtogroup Fonts
 * @{
 **/

#ifndef IRON_CLAD__FONTS__FONT_HPP
#define IRON_CLAD__FONTS__FONT_HPP

#include <map>
#include <sstream>
#include <string>

#include "ft2build.h"
#include "freetype/freetype.h"
#include "freetype/ftglyph.h"

#include "IronClad/Base/Errors.hpp"
#include "IronClad/Base/Types.hpp"

#include "IronClad/Graphics/Vertexbuffer.hpp"
#include "IronClad/Graphics/Globals.hpp"
#include "IronClad/Entity/Entity.hpp"

#include "Glyph.hpp"

namespace ic
{
namespace gui
{
    class IRONCLAD_API CFont
    {
    public:
        CFont();
        ~CFont();

        /**
         * Initializes the FreeType library.
         *  This should be called prior to ANY other font operations.
         *  Since it is static, no instance of CFont needs to be present,
         *  simply do:
         *      if(!CFont::Initialize())
         *          std::cerr << "FreeType 2 failed to initialize!\n";
         *
         * @return  TRUE on success, FALSE on failure.
         **/
        static bool Initialize();
        static void DeInitialize();

        /**
         * Loads a TrueType font.
         *  Here, all of the printable characters in the ASCII table
         *  are loaded as bitmaps, then are turned into render-able
         *  textures in GPU memory. Their dimensions are stored for
         *  rendering later on.
         *
         * @param   std::string     Font filename
         * @param   uint16_t        Font size
         *
         * @pre     CFont::Initialize() must have been called.
         *
         * @return  TRUE if everything went smoothly,
         *          FALSE otherwise.
         **/
        bool LoadFromFile(const std::string& filename, const uint16_t size);

        /**
         * Resizes the font.
         *  This stores a new font size and calls LoadFromFile()
         *  again, regenerating new bitmaps and textures for all
         *  characters.
         *  Since this can be a relatively expensive operation,
         *  it should be used sparingly. If you think you're going
         *  to be resizing a font often, consider caching multiple
         *  instances of CFont.
         *
         * @param   uint16_t    New font size
         *
         * @return  Value of CFont::LoadFromFile()
         **/
        bool Resize(const uint32_t size);

        /**
         * Renders text on the current frame buffer.
         *  Given a string, this function will render it at 
         *  the given position. A VBO will be created, vertex
         *  information generated, and then each character will
         *  be rendered using its texture.
         *  The position parameter marks the bottom of the "line"
         *  to be drawn on, so parts of characters such as 'j'
         *  or 'q' will fall below the line. Think of a sheet of
         *  paper.
         *  A rectangle is returned, in case it is useful to know
         *  the overall dimensions of the text rendered.
         *
         * @param   std::string     Text to render
         * @param   math::vector2_t Position to start rendering
         *
         * @return  Rectangle representing rendered text dimensions.
         *
         * @see GetTextWidth()
         * @see GetTextHeight()
         **/
        math::rect_t RenderText(const std::string& text,
                                const math::vector2_t& Pos);

        /**
         * Renders a string stored with CacheText().
         *  This method is much more efficient than a generic call to
         *  RenderText(). This is largely due to the fact that the glyph
         *  vertices are already stored in a vertex buffer and can thus
         *  immediately begin to be rendered. If you only have a single
         *  string to be rendered, you should use this method.
         *  
         * @return  Rectangle representing rendered text dimensions.
         **/
        math::rect_t RenderCached();

        /**
         * Caches a string of text in a buffer for rendering.
         *  This will store a series of glyph vertices in a vertex
         *  buffer for later immediate rendering.
         *
         * @param   std::string     Text to render
         * @param   math::vector2_t Position to start rendering
         *
         * @return  Rectangle representing rendered text dimensions.
         *
         * @see GetTextWidth()
         * @see GetTextHeight()
         * @see RenderText()
         **/
        math::rect_t CacheText(const std::string& text,
                               const math::vector2_t& Pos);

        bool IsLoaded() const { return m_loaded; }

        void SetFontColor(const color4f_t& Color);
        void SetFontColor(const float r, const float g, const float b);

        void SetProjection(const uint16_t w, const uint16_t h, 
                           const uint16_t max_z = 10,
                           const int min_z      = -10);

        void SetProjection(const math::matrix4x4_t& Proj);

        uint32_t GetTextWidth(const char* text)  const;
        uint32_t GetTextHeight(const char* text) const;

        const Glyph& GetGlyph(const char letter) const;

    private:
        static FT_Library   s_Library;
        static bool         s_initialized;

        ic::color4f_t       m_Color;
        gfx::CEffect        m_FontRender;
        gfx::CVertexBuffer  m_VBO, m_Cache;
        math::rect_t        m_CacheSize;

        FT_Face             m_FontFace;

        std::map<char, Glyph> mp_glyphTextures;
        
        std::string m_last_text;
        std::string m_filename;
        uint16_t    m_size;
        
        bool        m_loaded;
    };

    // I doubt this works :D

    static obj::CEntity* EntityFromText(const CFont& Font,
                                        gfx::CVertexBuffer& VBO,
                                        const char* text)
    {
        uint32_t width  = Font.GetTextWidth (text);
        uint32_t height = Font.GetTextHeight(text);
        
        obj::CEntity* pFinal = new obj::CEntity;
        asset::CMesh* pQuad  = asset::CAssetManager
                                    ::Create<asset::CMesh>();
        asset::CTexture* pTex= asset::CAssetManager
                                    ::Create<asset::CTexture>();

        vertex2_t vertices[4];
        vertices[0].Position = math::vector2_t(0,     0);
        vertices[1].Position = math::vector2_t(width, 0);
        vertices[2].Position = math::vector2_t(0    , height);
        vertices[3].Position = math::vector2_t(width, height);

        vertices[0].TexCoord = math::vector2_t(0, 0);
        vertices[1].TexCoord = math::vector2_t(1, 0);
        vertices[2].TexCoord = math::vector2_t(1, 1);
        vertices[3].TexCoord = math::vector2_t(0, 1);

        pQuad->LoadFromRaw(vertices, 4, 
                           gfx::Globals::g_FullscreenIndices, 6);

        pFinal->LoadFromMesh(pQuad, VBO);

        // Make lotsa buffers.
        struct Buffer { unsigned char* pBuf; uint32_t w; };

        std::vector<Buffer> allBuffers;
        allBuffers.resize(strlen(text));

        for(size_t i = 0; i < strlen(text); ++i)
        {
            Buffer buf;
            const Glyph& G = Font.GetGlyph(text[i]);
            G.pTexture->Bind();
            buf.pBuf = new unsigned char[G.pTexture->GetW() *
                                         G.pTexture->GetH() * 4];
            buf.w    = G.pTexture->GetW();
            glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, buf.pBuf);
            allBuffers.push_back(buf);
            G.pTexture->Unbind();
        }

        // Copy data from each glyph to the whole string texture.
        uint32_t tex_id = 0;
        glGenTextures(1, &tex_id);
        glBindTexture(GL_TEXTURE_2D, tex_id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, 
                     GL_RGBA, GL_UNSIGNED_BYTE, NULL);

        size_t offset = 0;
        for(size_t i = 0; i < allBuffers.size(); ++i)
        {
            std::stringstream ss; 
            ss << text[i];

            glTexSubImage2D(GL_TEXTURE_2D, 0, offset, 0,
                            Font.GetTextWidth(ss.str().c_str()),
                            Font.GetTextHeight(ss.str().c_str()),
                            GL_RGBA, GL_UNSIGNED_BYTE, allBuffers[i].pBuf);


            delete[] allBuffers[i].pBuf;

            offset += allBuffers[i].w;
        }

        glBindTexture(GL_TEXTURE_2D, 0);

        pTex->LoadFromTexture(tex_id);
        pFinal->SetMaterialOverride(pTex);

        return pFinal;
    }

}   // namespace font
}   // namespace ic

#endif // IRON_CLAD__FONTS__FONT_HPP

/** @} **/
