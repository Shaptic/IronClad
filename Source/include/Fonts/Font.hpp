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

#include "ft2build.h"
#include "freetype/freetype.h"

#include "Base/Errors.hpp"
#include "Base/Types.hpp"

#include "Graphics/Framebuffer.hpp"
#include "Entity/Entity.hpp"

namespace ic
{
namespace font
{
    class CFont
    {
    public:
        CFont();
        ~CFont();

        static bool Initialize()
        {
            return (FT_Init_FreeType(&m_Library) == 0);
        }

        bool LoadFromFile(const char* pfilename);
        bool LoadFromFile(const std::string& filename);

        bool Resize(const uint32_t size);

        gfx::CFrameBuffer* RenderText(const std::string& text);

    private:
        static FT_Library m_Library;

        FT_Face m_FontFace;
        bool m_loaded;
    };

}   // namespace font
}   // namespace ic

#endif // IRON_CLAD__FONTS__FONT_HPP

/** @} **/