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

namespace ic
{
    class CLevel
    {
    public:
        CLevel(gfx::CWindow& Window) : m_Window(Window)
        {
            mp_levelEntities.clear();
            m_filename.clear();
        }

        ~CLevel()
        {
            mp_levelEntities.clear();
        }

        bool LoadFromFile(const std::string& filename, gfx::CScene& Scene)
        {
            std::ifstream file;
            std::string line;

            file.open(filename);
            if(!file.is_open()) return false;

            while(std::getline(file, line))
            {
                if(line.empty() || line[0] == '/') continue;

                if(line == "[entity]")
                {
                    CEntity* pEntity    = NULL;
                    gfx::CLight* pLight = NULL;

                    while(std::getline(file, line) && line != "[end]")
                    {
                        if(line == "[mesh]")
                        {
                            const std::streampos start = file.tellg();

                            // Calculate mesh stream size.
                            while(std::getline(file, line) && line != "[endm]");
                            const std::streampos end = file.tellg();

                            // Reached eof, meaning last mesh in file.
                            if(end == std::streampos(std::ios::end))
                            {
                                file.close();
                                file.open(filename);
                            }

                            file.seekg(start);

                            // Load mesh from existing stream.
                            asset::CMesh* pMesh = 
                                asset::CAssetManager::Create<asset::CMesh>();

                            if(!pMesh->LoadFromExisting(file, end))
                            {
                                asset::CAssetManager::Destroy<asset::CMesh>(pMesh);
                                return false;
                            }

                            if(!pEntity->LoadFromMesh(pMesh, Scene.m_GeometryVBO))
                                return false;

                            Scene.AddMesh(pEntity);
                            mp_levelEntities.push_back(pEntity);
                        }

                        else if(line == "[light]")
                        {
                            if(pLight == NULL) return false;

                            pLight->Enable();

                            while(std::getline(file, line) && line != "[endl]")
                            {
                                if(line.find("position") == 0)
                                {
                                    std::vector<std::string> splitLines = 
                                        util::split(line, '=');

                                    if(splitLines.size() != 2) return false;

                                    splitLines = util::split(splitLines[1], ',');

                                    if(splitLines.size() != 2) return false;
                                    pLight->SetPosition(
                                        pEntity->GetX() + atof(splitLines[0].c_str()),
                                        pEntity->GetY() + atof(splitLines[1].c_str()));

                                }
                                else if(line.find("color") == 0)
                                {
                                    std::vector<std::string> splitLines = 
                                        util::split(line, '=');

                                    if(splitLines.size() != 2) return false;

                                    splitLines = util::split(splitLines[1], ',');

                                    if(splitLines.size() != 3) return false;
                                    pLight->SetColor(
                                        atof(splitLines[0].c_str()),
                                        atof(splitLines[1].c_str()),
                                        atof(splitLines[2].c_str()));
                                }
                                else if(line.find("attenuation") == 0)
                                {
                                    std::vector<std::string> splitLines = 
                                        util::split(line, '=');

                                    if(splitLines.size() != 2) return false;

                                    splitLines = util::split(splitLines[1], ',');

                                    if(splitLines.size() != 3) return false;
                                    pLight->SetAttenuation(
                                        atof(splitLines[0].c_str()),
                                        atof(splitLines[1].c_str()),
                                        atof(splitLines[2].c_str()));
                                }
                                else if(line.find("brightness") == 0)
                                {
                                    std::vector<std::string> splitLines = 
                                        util::split(line, '=');

                                    if(splitLines.size() != 2) return false;

                                    pLight->SetBrightness(
                                        atof(splitLines[1].c_str()));
                                }
                            }

                            pLight->Disable();
                            Scene.AddLight(pLight);
                        }

                        else if(line.find("attribs") == 0)
                        {
                            // attribs=a,b,c,d,e,f
                            std::vector<std::string> splitLines = util::split(line, '=');
                            if(splitLines.size() != 2) return false;

                            splitLines = util::split(splitLines[1], ',');
                            
                            for(size_t attrib = 0;
                                attrib < splitLines.size();
                                ++attrib)
                            {
                                int value = atoi(splitLines[attrib].c_str());

                                // Physics
                                if(attrib == 0 && value == 1)
                                {
                                    pEntity = new CRigidBody;
                                }
                                // Motion
                                else if(attrib == 1 && value == 1)
                                {
                                }
                                // Light source
                                else if(attrib == 2 && value == 1)
                                {
                                    pLight = new gfx::CLight;
                                    pLight->Init(gfx::IC_POINT_LIGHT, m_Window);
                                }
                                // Player spawn / enemy spawn / etc.
                                else
                                {

                                }
                            }

                            if(pEntity == NULL) pEntity = new CEntity;
                        }

                        else if(line.find("location") != std::string::npos)
                        {
                            // location=x,y
                            std::vector<std::string> splitLines = util::split(line, '=');
                            if(splitLines.size() != 2) return false;

                            splitLines = util::split(splitLines[1], ',');
                            float x = atof(splitLines[0].c_str());
                            float y = atof(splitLines[1].c_str());

                            if(pEntity != NULL) pEntity->Move(x, y);
                        }
                    }
                }
            }

            file.close();
            m_filename = filename;            
            return true;
        }

        std::vector<CEntity*> mp_levelEntities;

    private:
        static const uint8_t   IC_LVL_INACTIVE      = 0x01;
        static const uint8_t   IC_LVL_ACTIVE        = 0x02;
        static const uint8_t   IC_LVL_RIGID_BODY    = 0x04;
        static const uint8_t   IC_LVL_PLAYER_SPAWN  = 0x08;
        static const uint8_t   IC_LVL_ENEMY_SPAWN   = 0x16;
        static const uint8_t   IC_LVL_LIGHT         = 0x32;

        gfx::CWindow& m_Window;

        std::string m_filename;
    };
}

#endif // IRON_CLAD__LEVEL_HPP

/** @} **/