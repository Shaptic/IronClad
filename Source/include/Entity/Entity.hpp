/**
 * @file
 *  Entity.hpp - Defines the high-level CEntity class, which wraps up
 *  CMeshInstance's in a more user-friendly interface.
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
 * @addtogroup Engine
 * @{
 **/

#ifndef IRON_CLAD__ENTITY_HPP
#define IRON_CLAD__ENTITY_HPP

#include "Base/Types.hpp"
#include "Math/Math.hpp"
#include "Graphics/MeshInstance.hpp"

namespace ic
{
    /**
     * A wrapper for the mesh instances with additional functionality.
     * 
     * @todo    Calculate width and height properly.
     * @warning CEntity::GetTexture() has potential for out-of-bounds.
     **/
    class IRONCLAD_API CEntity
    {
    public:
        CEntity(bool caster = false) : m_render(true), m_casts(caster) {}
        ~CEntity();

        /**
         * Loads a mesh instance into the entity.
         * 
         * @param   char*           Mesh filename
         * @param   CVertexBuffer&  VBO to load mesh data into
         * 
         * @return  TRUE if loaded, FALSE if not.
         **/
        bool LoadFromFile(const char* pmesh_filename,
            gfx::CVertexBuffer& VBO);

        inline bool LoadFromFile(const std::string& mesh_filename,
            gfx::CVertexBuffer& VBO)
        { return this->LoadFromFile(mesh_filename.c_str(), VBO); }

        /**
         * Loads a mesh instance from an un-offloaded mesh.
         * 
         * @param   CMesh*          Mesh to load
         * @param   CVertexBuffer&  VBO to load mesh into
         * 
         * @return  TRUE on success, FALSE on error.
         **/
        inline bool LoadFromMesh(asset::CMesh* pMesh, gfx::CVertexBuffer& VBO)
        {
            m_Mesh.mp_ActiveMesh = pMesh;
            return m_Mesh.LoadIntoVBO(VBO);
        }
    
        /**
         * Moves the entity to a location.
         * @param   vector2_t&  New position
         **/
        inline void Move(const math::vector2_t& Pos)
        { m_Mesh.Move(Pos); }

        void Move(const float x, const float y);

        /**
         * Moves the entity at a rate.
         * @param   vector2_t&  Movement rate
         **/
        void Adjust(const math::vector2_t& Rate);
        void Adjust(const float dx, const float dy)
        {
            this->Move(this->GetMesh().GetPosition().x + dx,
                       this->GetMesh().GetPosition().y + dy);
        }

        /**
         * Checks for a collision.
         * @param   T&  Object to check collision with
         * @return  TRUE if collide, FALSE if not.
         * @todo    Implement a CCollidable in a separate Physics lib.
         **/
        //bool CollidesWith(const CEntity& Object);
        //bool CollidesWith(const CEntity* const pObject);
        //bool CollidesWith(const math::vector2_t& Location);
        //bool CollidesWith(const math::Rect& Rectangle);
        //bool CollidesWith(const math::Rectf& Rectangle);
        
        /**
         * Toggles rendering.
         *  Default is TRUE, of course. If set to FALSE, the CScene it's
         *  attached to will not render this entity.
         *  
         * @return  The old state (TRUE if not rendering, etc).
         **/
        inline bool ToggleRendering()
        { return !(m_render = !m_render); }

        inline bool IsRenderable() const
        { return m_render; }

        inline bool CastsShadow() const
        { return m_casts; }

        inline gfx::CMeshInstance& GetMesh()
        { return m_Mesh; }

        inline const math::vector2_t& GetPosition() const
        { return m_Mesh.GetPosition(); }

        inline int GetX() const 
        { return this->GetPosition().x; }

        inline int GetY() const 
        { return this->GetPosition().y; }

        inline void SetShadowCasting(bool flag)
        { m_casts = flag; }

        inline void SetMesh(asset::CMesh* pMesh)
        { m_Mesh.mp_ActiveMesh = pMesh; }

        /// @todo   Bounds checking hehe.
        inline asset::CTexture* GetTexture() const
        { return m_Mesh.GetSurfaces()[0]->pMaterial->pTexture; }

    private:
        gfx::CMeshInstance  m_Mesh;
        //math::Rect      m_CollisionBox;

        bool m_render, m_casts;
    };
}

#endif // IRON_CLAD__ENTITY_HPP

/** @} **/