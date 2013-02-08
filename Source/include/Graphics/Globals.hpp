/**
 * @file
 *	Graphics/Globals.hpp - Contains static pseudo-global variables that
 *	are used in multiple subsystems.
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
 * @addtogroup Graphics
 * @{
 **/

#ifndef IRON_CLAD__GRAPHICS__GLOBALS_HPP
#define IRON_CLAD__GRAPHICS__GLOBALS_HPP

#include "Base/Types.hpp"

#include "Asset/Texture.hpp"
#include "Graphics/Window.hpp"
#include "Graphics/VertexBuffer.hpp"
#include "Graphics/Effect.hpp"

namespace ic
{
namespace gfx
{
    struct IRONCLAD_API Globals
    {        
        /**
         * @todo    Do NOT hardcore window size.
         **/
        static bool Init(CWindow& Window);
        static void LoadVBODefaults();
        static void LoadVBOCustom(const color4f_t& Color);

        static CEffect          g_DefaultEffect;
        static CVertexBuffer    g_FullscreenVBO;

        static asset::CTexture* g_WhiteTexture;

        static vertex2_t        g_FullscreenVertices[4];
        static uint16_t         g_FullscreenIndices[6];
    };

}   // namespace gfx
}   // namespace ic

#endif // IRON_CLAD__GRAPHICS__GLOBALS_HPP

/** @} **/