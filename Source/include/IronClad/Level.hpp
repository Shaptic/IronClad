/**
 * @file
 *  Level.hpp - Defines a loader for .iclvl files.
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
 * @addtogroup Engine
 * @{
 **/

#ifndef IRON_CLAD__LEVEL_HPP
#define IRON_CLAD__LEVEL_HPP

#include <string>
#include <fstream>

#include "IronClad/Asset/AssetManager.hpp"
#include "IronClad/Asset/Mesh.hpp"
#include "IronClad/Graphics/Scene.hpp"
#include "IronClad/Entity/Animation.hpp"

namespace ic
{
    /**
     * A level loader class.
     *  This class loads and parses .iclvl files. It will generate lights
     *  and mesh instances for objects in the level.
     * 
     * @todo    Add control of physics-enabled objects
     * @see     Docs/ICLvl.spec
     **/
    class IRONCLAD_API CLevel
    {
    public:
        CLevel(const gfx::CWindow& Window);
        ~CLevel();

        bool LoadFromFile(const std::string& filename, gfx::CScene& Scene);

        void Update()
        {
            for(size_t i = 0; i < mp_lvlAnimations.size(); ++i)
                mp_lvlAnimations[i]->Update();
        }

        const math::vector2_t& GetPlayerSpawn() const
        {
            return m_PlayerSpawn;
        }

        const std::vector<obj::CAnimation*>& GetAnimatableObjects() const
        { return mp_lvlAnimations; }

        const std::vector<obj::CRigidBody*>& GetPhysicalObjects() const
        { return mp_lvlBodies; }

        const std::vector<obj::CEntity*>& GetOtherObjects() const
        { return mp_lvlOther; }

        const std::vector<gfx::CLight*>& GetLights() const
        { return mp_lvlLights; }

        std::vector<obj::CEntity*> mp_levelEntities;

    private:
        template<typename T>
        void Clear(std::vector<T*>& data);

        std::vector<obj::CAnimation*>   mp_lvlAnimations;
        std::vector<obj::CRigidBody*>   mp_lvlBodies;
        std::vector<obj::CEntity*>      mp_lvlOther;
        std::vector<gfx::CLight*>       mp_lvlLights;
        std::vector<math::vector2_t>    m_lvlSpawns;

        math::vector2_t                 m_PlayerSpawn;
        const gfx::CWindow& m_Window;

        std::string m_filename;
    };

    template<typename T>
    void CLevel::Clear(std::vector<T*>& data)
    {
        //for(size_t i = 0; i < data.size(); ++i)
        //    delete data[i];
        data.clear();
    }
}

#endif // IRON_CLAD__LEVEL_HPP

/** @} **/