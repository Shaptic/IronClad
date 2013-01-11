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

        void AddVerticalForce(const float dy)
        {
            m_vert += dy;
        }

        void AddHorizontalForce(const float dx)
        {
            m_horz += dx;
        }

        void Equilibrium()
        {
            m_horz = m_vert = 0.f;
        }

        void Update()
        {
            this->Adjust(m_horz, m_vert);
        }

    private:
        float m_vert, m_horz;
    };
}

#endif // IRON_CLAD__ENTITY__RIGID_BODY_HPP

/** @} **/