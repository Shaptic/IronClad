/**
 * @file
 *	Level.hpp - Defines a loader for .iclvl files.
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
 * @addtogroup Engine
 * @{
 **/

#ifndef IRON_CLAD__LEVEL_HPP
#define IRON_CLAD__LEVEL_HPP

#include <string>
#include <fstream>

#include "Asset/AssetManager.hpp"
#include "Asset/Mesh.hpp"
#include "Graphics/Scene.hpp"

namespace ic
{
    /**
     * A level loader class.
     *  This class loads and parses .iclvl files. It will generate lights
     *  and mesh instances for objects in the level.
     * 
     * @todo    Add control of physics-enabled objects
     * 
     * @see     Docs/ICLvl.spec
     **/
    class IRONCLAD_API CLevel
    {
    public:
        CLevel(gfx::CWindow& Window);

        ~CLevel();

        bool LoadFromFile(const std::string& filename, gfx::CScene& Scene);

        const math::vector2_t& GetPlayerSpawn() const
        {
            return mp_PlayerSpawn;
        }

        const std::vector<CRigidBody*>& GetPhysicalEntities() const
        {
            return mp_levelPhysics;
        }

        const std::vector<CEntity*>& GetEnemySpawns() const
        {
            return mp_levelESpawns;
        }

        const std::vector<gfx::CLight*>& GetLights() const
        {
            return mp_levelLights;
        }

        std::vector<CEntity*> mp_levelEntities;
    private:
        //static const uint8_t   IC_LVL_INACTIVE      = 0x01;
        //static const uint8_t   IC_LVL_ACTIVE        = 0x02;
        //static const uint8_t   IC_LVL_RIGID_BODY    = 0x04;
        //static const uint8_t   IC_LVL_PLAYER_SPAWN  = 0x08;
        //static const uint8_t   IC_LVL_ENEMY_SPAWN   = 0x16;
        //static const uint8_t   IC_LVL_LIGHT         = 0x32;

        std::vector<CRigidBody*>    mp_levelPhysics;
        std::vector<CEntity*>       mp_levelESpawns;
        std::vector<gfx::CLight*>   mp_levelLights;
        math::vector2_t             mp_PlayerSpawn;

        gfx::CWindow& m_Window;

        std::string m_filename;
    };
}

#endif // IRON_CLAD__LEVEL_HPP

/** @} **/