/**
 * @file
 *  Graphics/Surface.hpp - Defines the surface_t type.
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

#ifndef IRON_CLAD__GRAPHICS__SURFACE_HPP
#define IRON_CLAD__GRAPHICS__SURFACE_HPP

#include "IronClad/Base/Types.hpp"
#include "IronClad/Graphics/Material.hpp"

namespace ic
{
namespace gfx
{
    /**
     * Describes a surface on a mesh.
     *  Surfaces help define various parts of a mesh, providing useful
     *  data such as the material associated with it, the quantity of
     *  vertices / indices it encompasses, and the offsets where that
     *  data is located.
     *  On loading of a mesh, the CMesh will do surface-merging,
     *  which compresses all of the surfaces that share material_t's into a
     *  single surface, allowing for rendering to be done in much
     *  larger batches.
     **/
    struct IRONCLAD_API surface_t
    {
        static bool SortByMaterial(const surface_t* pOne, 
            const surface_t* pTwo)
        {
            gfx::CShaderPair* pS1 = pOne->pMaterial->pShader;
            gfx::CShaderPair* pS2 = pTwo->pMaterial->pShader;

            if(pS1 == pS2) return false;
            else if(!pS1) return true;
            else if(!pS2) return false;
            else return pS1->GetProgram() < pS2->GetProgram();
        }

        material_t* pMaterial;  // The material to bind for the surface.
        uint32_t    start;      // The starting point in the buffer.
        uint16_t    icount;     // The number of indices.
    };

}   // namespace gfx
}   // namespace ic

#endif // IRON_CLAD__GRAPHICS__SURFACE_HPP

/** @} **/