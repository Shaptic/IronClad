/**
 * @file
 *	Fonts/Font.hpp - A wrapper for an instance of a .ttf font using the FreeType 2 API.
 *
 * @author      George Kudrayvtsev (switch1440)
 * @version     1.0
 * @copyright   Apache License v2.0
 *  Licensed under the Apache License, Version 2.0 (the "License").			\n
 *  You may not use this file except in compliance with the License.		\n
 *  You may obtain a copy of the License at:
 *  http://www.apache.org/licenses/LICENSE-2.0 								\n
 *  Unless required by applicable law or agreed to in writing, software		\n
 *  distributed under the License is distributed on an "AS IS" BASIS,		\n
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\n
 *  See the License for the specific language governing permissions and		\n
 *  limitations under the License.
 *
 * @addtogroup Fonts
 * @{
 **/

#ifndef IRON_CLAD__FONTS__FONT_HPP
#define IRON_CLAD__FONTS__FONT_HPP

#include <map>

#include "ft2build.h"
#include "freetype/freetype.h"
#include "freetype/ftglyph.h"

#include "Base/Errors.hpp"
#include "Base/Types.hpp"

#include "Graphics/Vertexbuffer.hpp"
#include "Graphics/Globals.hpp"
#include "Entity/Entity.hpp"

namespace ic
{
namespace font
{
    class CFont
    {
    public:
        CFont(){}
        ~CFont(){}

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

        /**
         * Loads a TrueType font.
         *  Here, all of the printable characters in the ASCII table
         *  are loaded as bitmaps, then are turned into renderable
         *  textures in GPU memory. Their dimensions are stored for
         *  rendering later on.
         *
         * @param   std::string     Font filename
         * @paarm   uint16_t        Font size       (optional=12)
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
         * Renders text on the current framebuffer.
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

        void SetFontColor(const color4f_t& Color);
        void SetFontColor(const float r, const float g, const float b);

    private:
        static FT_Library   m_Library;
        static bool         m_initialized;

        struct Glyph
        {
            asset::CTexture* pTexture;
            math::rect_t     dim;
        };

        ic::color4f_t   m_Color;
        gfx::CEffect    m_FontRender;

        FT_Face         m_FontFace;

        std::map<char, Glyph> mp_glyphTextures;
        
        std::string m_filename;
        uint16_t    m_size;
        
        bool        m_loaded;
    };

}   // namespace font
}   // namespace ic

#endif // IRON_CLAD__FONTS__FONT_HPP

/** @} **/