/**
 * @file
 *  Graphics/Material.hpp - Declarations for a CShader OpenGL shader wrapper.
 *
 * @author      George Kudrayvtsev (halcyon)
 * @version     1.0
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
 * @addtogroup Graphics
 * @{
 **/

#ifndef IRON_CLAD__GRAPHICS__MATERIAL_HPP
#define IRON_CLAD__GRAPHICS__MATERIAL_HPP

#include "IronClad/Asset/Texture.hpp"
#include "ShaderPair.hpp"

namespace ic
{
namespace gfx
{
    /**
     * A texture / shader combination.
     *  In the IronClad engine, a material is defined as being a texture
     *  combined with a shader. These can be mutually exclusive: a material
     *  can be just a texture with no shader effects, or just a shader 
     *  with no texture bound to it.
     **/
    struct IRONCLAD_API material_t
    {
        material_t() : pShader(NULL), pTexture(NULL) {}
        inline bool Bind()
        { 
            if(pTexture)    pTexture->Bind();
            if(pShader)     pShader->Bind();
            return (pShader != NULL);
        }

        inline bool Unbind()
        {
            if(pTexture)    pTexture->Unbind();
            if(pShader)     pShader->Unbind();
            return (pShader  != NULL);
        }

        gfx::CShaderPair*   pShader;
        asset::CTexture*    pTexture;
    };

}   // namespace gfx
}   // namespace ic

#endif // IRON_CLAD__GRAPHICS__MATERIAL_HPP

/** @} **/