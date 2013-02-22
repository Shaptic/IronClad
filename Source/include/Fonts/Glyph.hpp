/**
 * @file
 *	Fonts/Glyph.hpp - Defines a structure to hold glyph data.
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

#ifndef IRON_CLAD__FONTS__GLYPH_HPP
#define IRON_CLAD__FONTS__GLYPH_HPP

#include "Asset/Texture.hpp"
#include "Math/Shapes.hpp"

namespace ic
{
namespace font
{
    /**
     * A glyph structure represents a single letter in a font file.
     **/
    struct Glyph
    {
        asset::CTexture* pTexture;
        math::rect_t     dim;
    };
}
}

#endif // IRON_CLAD__FONTS__GLYPH_HPP

/** @} **/