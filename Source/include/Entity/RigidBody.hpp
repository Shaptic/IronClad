/**
 * @file
 *	Entity/RigidBody.hpp - The basic physics-controlled entity in the game world.
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
 * @addtogroup Entity
 * @{
 **/

#ifndef IRON_CLAD__ENTITY__RIGID_BODY_HPP
#define IRON_CLAD__ENTITY__RIGID_BODY_HPP

#include "Entity.hpp"

namespace ic
{
    class CRigidBody : public CEntity
    {
    public:
        CRigidBody() : m_vert(0.f), m_horz(0.f) {}

        ~CRigidBody(){}

        bool LoadFromFile(const char* pmesh_filename,
            gfx::CVertexBuffer& VBO)
        {
            bool val = CEntity::LoadFromFile(pmesh_filename, VBO);

            m_CollisionBox.w = CEntity::GetW();
            m_CollisionBox.h = CEntity::GetH();

            return val;
        }

        bool LoadFromFile(const std::string& mesh_filename,
            gfx::CVertexBuffer& VBO)
        {
            bool val = CEntity::LoadFromFile(mesh_filename, VBO);
            
            m_CollisionBox.w = CEntity::GetW();
            m_CollisionBox.h = CEntity::GetH();

            return val;
        }

        bool LoadFromMesh(asset::CMesh* pMesh, gfx::CVertexBuffer& VBO)
        {
            bool val = CEntity::LoadFromMesh(pMesh, VBO);

            m_CollisionBox.w = CEntity::GetW();
            m_CollisionBox.h = CEntity::GetH();

            return val;
        }

        void CreateCollisionRect(const math::rect_t& Rect);
        void CreateCollisionRect(const int w, const int h);
        void CreateCollisionRect(const math::vector2_t& Pos, const int w, const int h);
        void CreateCollisionRect(const float x, const float y, const int w, const int h);

        void Move(const math::vector2_t& Pos)
        {
            CEntity::Move(Pos);

            m_CollisionBox.x = Pos.x;
            m_CollisionBox.y = Pos.y;
        }

        void Move(const float x, const float y)
        {
            CEntity::Move(x, y);

            m_CollisionBox.x = x;
            m_CollisionBox.y = y;
        }

        void Adjust(const math::vector2_t& Rate)
        {
            CEntity::Adjust(Rate);

            m_CollisionBox.x += Rate.x;
            m_CollisionBox.y += Rate.y;
        }
        void Adjust(const float dx, const float dy)
        {
            CEntity::Adjust(dx, dy);

            m_CollisionBox.x += dx;
            m_CollisionBox.y += dy;
        }

        void AddVForce(const float dy)
        {
            m_vert += dy;
        }

        void AddHForce(const float dx)
        {
            m_horz += dx;
        }

        void Equilibrium()
        {
            m_horz = m_vert = 0.f;
        }

        bool CheckCollision(const CEntity* Other) const
        {
            math::rect_t OtherBox;
            OtherBox.x = Other->GetX();
            OtherBox.y = Other->GetY();
            OtherBox.w = Other->GetW();
            OtherBox.h = Other->GetH();

            return util::CheckCollision(m_CollisionBox, OtherBox);            
        }
        bool CheckCollision(const math::rect_t& Other) const
        {
            return util::CheckCollision(m_CollisionBox, Other);
        }

        void Update()
        {
            this->Adjust(m_horz, m_vert);
        }

    private:
        math::rect_t m_CollisionBox;
        float m_vert, m_horz;
    };
}

#endif // IRON_CLAD__ENTITY__RIGID_BODY_HPP

/** @} **/