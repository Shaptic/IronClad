/**
 * @file
 *  Graphics/MeshInstance.hpp - Declarations of the CMeshInstance class,
 *  responsible for representing a unique instance of a CMesh object.
 *
 * @author      George Kudrayvtsev (switch1440)
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

#ifndef IRON_CLAD__GRAPHICS__MESH_INSTANCE_HPP
#define IRON_CLAD__GRAPHICS__MESH_INSTANCE_HPP

#include "Material.hpp"
#include "VertexBuffer.hpp"
#include "Asset/Mesh.hpp"

namespace ic
{
    class CEntity;

namespace gfx
{
    /**
     * An instance of a vertex mesh. 
     *  This class merely contains a pointer to the CMesh it uses,
     *  some material overrides if necessary, and transformations for
     *  this unique instance of the mesh.
     *  Once a mesh instance is created, the matrices for it can be passed
     *  on to the vertex shader in order to render the same mesh vertices
     *  in various places.
     **/
    class IRONCLAD_API CMeshInstance
    {
    public:
        CMeshInstance() : mp_ActiveMesh(NULL) {}
        ~CMeshInstance() {}

        /**
         * Loads a mesh asset.
         * @param   CMesh*  Mesh to load.
         * @return  TRUE on successful load, FALSE on error.
         **/
        bool LoadMesh(asset::CMesh* pMesh);
        bool LoadMesh(const std::string& filename);
        bool LoadMesh(const vertex2_t* verts, const uint16_t vsize,
                      const uint16_t*  inds,  const uint16_t isize);

        /**
         * Loads the current instance into the GPU.
         *  Since this may not be the only instance of the mesh, if it has
         *  already been loaded into the GPU (e.g. CMesh::m_VertexBuffer
         *  is empty) then nothing is done.
         *
         * @param   CVertexBuffer&  The VBO to load into
         * 
         * @return  TRUE if the mesh was loaded in, FALSE if it's empty.
         **/
        bool LoadIntoVBO(gfx::CVertexBuffer& VBO);

        /**
         * Moves the mesh instance.
         * @param   math::vector2_t&    Position to move to
         **/
        void Move(const math::vector2_t& Pos)
        {
            m_Position.x = Pos.x;
            m_Position.y = Pos.y;
        }

        /**
         * Loads instance position data into an existing model-view matrix.
         *  The old system used to have each CMeshInstance contain its own
         *  model-view matrix, and simply do a call to GetPositionMatrix(),
         *  but that was a serious waste of RAM. Now, the CScene using this
         *  instance just provides a single matrix and the position is
         *  loaded into it. Each CMeshInstance now stores 2 floats for
         *  position instead of 16. That's a savings of 56 bytes ;)
         * 
         * @param   math::matrix4x4_t&  Model-view matrix to load into
         **/
        inline void LoadPositionMatrix(math::matrix4x4_t& MVMatrix)
        { MVMatrix[0][3] = m_Position.x; MVMatrix[1][3] = m_Position.y; }

        inline std::vector<gfx::surface_t*>& GetSurfaces() const
        { return mp_ActiveMesh->mp_Surfaces; }

        inline const math::vector2_t& GetPosition() const
        { return m_Position; }

        inline const math::vector2_t& GetDimensions() const
        { return m_Dimensions; }

        friend class CEntity;

    private:
        asset::CMesh*       mp_ActiveMesh;
        math::vector2_t     m_Position;
        math::vector2_t     m_Dimensions;
    };

}   // namespace gfx
}   // namespace ic

#endif // IRON_CLAD__GRAPHICS__MESH_INSTANCE_HPP

/** @}  */
