/**
 * @file
 *  Entity.hpp - Defines the high-level obj::CEntity class, which wraps up
 *  CMeshInstance's in a more user-friendly interface.
 *
 * @author      George Kudrayvtsev (halcyon)
 * @version     1.1
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

#include "IronClad/Base/Types.hpp"
#include "IronClad/Math/Math.hpp"
#include "IronClad/Graphics/MeshInstance.hpp"

namespace ic
{
namespace obj
{
    enum RotationAxis
    {
        IC_X_AXIS,
        IC_Y_AXIS,
        IC_Z_AXIS
    };

    /**
     * A wrapper for the mesh instances with additional functionality.
     * 
     * @todo    Calculate width and height properly.
     * @warning CEntity::GetTexture() has potential for out-of-bounds.
     **/
    class IRONCLAD_API CEntity
    {
    public:
        CEntity(bool caster = false) : m_render(true), mp_Override(NULL),
                                       m_static(false) {}
        virtual ~CEntity();

        inline bool operator==(const std::string& filename) const
        { return (this->GetTexture()->GetFilename() == filename); }

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
         * Dynamically loads a quad mesh from a texture.
         * @param  char*            Texture filename
         * @param   CVertexBuffer&  VBO to load mesh data into
         * 
         * @return  TRUE if loaded, FALSE if not.
         **/
        virtual bool LoadFromImage(const char* pimg_name,
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

        virtual void Rotate(const float degrees, const RotationAxis axis)
        {
            switch(axis)
            {
            case IC_X_AXIS:
                m_Mesh.RotateX(degrees);
                break;

            case IC_Y_AXIS:
                m_Mesh.RotateY(degrees);
                break;

            case IC_Z_AXIS:
                m_Mesh.RotateZ(degrees);
                break;
            }
        }
        virtual void AdjustRotation(const float dtheta, const RotationAxis axis)
        {
            switch(axis)
            {
            case IC_X_AXIS:
                m_Mesh.RotateX(m_Mesh.GetRotationX() + dtheta);
                break;

            case IC_Y_AXIS:
                m_Mesh.RotateY(m_Mesh.GetRotationY() + dtheta);
                break;

            case IC_Z_AXIS:
                m_Mesh.RotateZ(m_Mesh.GetRotationZ() + dtheta);
                break;
            }
        }

        void FlipV() { m_Mesh.VFlip(); }
        void FlipH() { m_Mesh.HFlip(); }
        
        // Does nothing, but could be implemented in inheriting classes.
        virtual void Update(){}

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

        inline void SetRendering(const bool flag)
        { m_render = flag; }

        inline void SetStatic(const bool flag)
        { m_static = flag; }

        inline bool IsRenderable() const
        { return m_render; }

        inline bool IsStatic() const 
        { return m_static; }

        inline bool HasOverride() const
        { return mp_Override != NULL; }

        inline gfx::CMeshInstance& GetMesh()
        { return m_Mesh; }

        inline asset::CTexture* GetOverride() const
        { return mp_Override; }

        inline const math::vector2_t& GetPosition() const
        { return m_Mesh.GetPosition(); }

        inline math::rect_t GetRect() const 
        { return math::rect_t(GetX(), GetY(), GetW(), GetH()); }

        inline math::vector2_t GetCenter() const
        { return math::vector2_t(GetX() + GetW() / 2.f, GetY() + GetH() / 2.f); }

        inline float GetX() const 
        { return this->GetPosition().x; }

        inline float GetY() const 
        { return this->GetPosition().y; }

        inline float GetD(const RotationAxis axis) const
        {
            if(axis == IC_X_AXIS) return m_Mesh.GetRotationX();
            if(axis == IC_Y_AXIS) return m_Mesh.GetRotationY();
            if(axis == IC_Y_AXIS) return m_Mesh.GetRotationZ();
            return 0.f;
        }

        inline int GetW() const
        { return m_Mesh.GetDimensions().x; }

        inline int GetH() const
        { return m_Mesh.GetDimensions().y; }

        inline void SetMesh(asset::CMesh* pMesh)
        { m_Mesh.mp_ActiveMesh = pMesh; }

        /// @todo   Bounds checking hehe.
        virtual asset::CTexture* GetTexture() const
        {
            if(!this->HasOverride())
                return m_Mesh.GetSurfaces()[0]->pMaterial->pTexture;
            else
                return this->GetOverride();
        }

    protected:
        gfx::CMeshInstance  m_Mesh;
        asset::CTexture*    mp_Override;

        bool m_render, m_static;
    };
}   // namespace obj
}   // namespace ic

#endif // IRON_CLAD__ENTITY_HPP

/** @} **/