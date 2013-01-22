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
        CEntity(bool caster = false) : m_render(true), m_casts(caster),
            mp_Override(NULL) {}
        ~CEntity();

        inline bool operator==(const std::string& filename) const
        { return this->GetTexture()->GetFilename() == filename; }

        /**
         * Loads a mesh instance into the entity.
         * 
         * @param   char*           Mesh filename
         * @param   CVertexBuffer&  VBO to load mesh data into
         * 
         * @return  TRUE if loaded, FALSE if not.
         **/
        virtual bool LoadFromFile(const char* pmesh_filename,
                                  gfx::CVertexBuffer& VBO);
        virtual bool LoadFromFile(const std::string& mesh_filename,
                                  gfx::CVertexBuffer& VBO);

        /**
         * Loads a mesh instance from an un-offloaded mesh.
         * 
         * @param   CMesh*          Mesh to load
         * @param   CVertexBuffer&  VBO to load mesh into
         * 
         * @return  TRUE on success, FALSE on error.
         **/
        virtual bool LoadFromMesh(asset::CMesh* pMesh,
                                  gfx::CVertexBuffer& VBO);
    
        /**
         * Moves the entity to a location.
         * @param   vector2_t&  New position
         **/
        virtual void Move(const math::vector2_t& Pos);
        virtual void Move(const float x, const float y);

        /**
         * Moves the entity at a rate.
         * @param   vector2_t&  Movement rate
         **/
        virtual void Adjust(const math::vector2_t& Rate);
        virtual void Adjust(const float dx, const float dy);
        
        /**
         * Enables a texture override over the default.
         *  This will cause the default texture(s) on the mesh to be
         *  ignored during rendering, and this one to be used instead.
         *  Could potentially be useful for sprite animation.
         * 
         * @param   CTexture*   Texture to override mesh with
         **/
        void SetMaterialOverride(asset::CTexture* pTexture);

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

        inline bool HasOverride() const
        { return mp_Override != NULL; }

        inline gfx::CMeshInstance& GetMesh()
        { return m_Mesh; }

        inline asset::CTexture* GetOverride() const
        { return mp_Override; }

        inline const math::vector2_t& GetPosition() const
        { return m_Mesh.GetPosition(); }

        inline int GetX() const 
        { return this->GetPosition().x; }

        inline int GetY() const 
        { return this->GetPosition().y; }

        inline int GetW() const
        { return this->m_Mesh.GetDimensions().x; }

        inline int GetH() const
        { return this->m_Mesh.GetDimensions().y; }

        inline void SetShadowCasting(bool flag)
        { m_casts = flag; }

        inline void SetMesh(asset::CMesh* pMesh)
        { m_Mesh.mp_ActiveMesh = pMesh; }

        /// @todo   Bounds checking hehe.
        inline asset::CTexture* GetTexture() const
        {
            if(!this->HasOverride())
                return m_Mesh.GetSurfaces()[0]->pMaterial->pTexture;
            else
                return this->GetOverride();
        }

    protected:
        gfx::CMeshInstance  m_Mesh;
        asset::CTexture*    mp_Override;

        bool m_render, m_casts;
    };
}

#endif // IRON_CLAD__ENTITY_HPP

/** @} **/