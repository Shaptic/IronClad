#include "IronClad/Level.hpp"

using namespace ic;

CLevel::CLevel(const gfx::CWindow& Window) : m_Window(Window)
{
    mp_levelEntities.clear();
    m_filename.clear();
}

CLevel::~CLevel()
{
    mp_levelEntities.clear();
}

bool CLevel::LoadFromFile(const std::string& filename, gfx::CScene& Scene)
{
    std::stringstream ss;
    std::ifstream file;
    std::string line;

    file.open(filename);
    if(!file.is_open()) return false;

    while(std::getline(file, line))
    {
        if(line.empty() || line[0] == '/') continue;

        if(line == "[entity]")
        {
            obj::CEntity* pEntity    = NULL;
            gfx::CLight* pLight = NULL;

            bool is_pspawn = false;
            bool is_espawn = false;

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
                    ss << filename << ":Mesh";
                    pMesh->SetFilename(ss.str());
                    ss.str(std::string());

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
                            pEntity = new obj::CRigidBody;
                            mp_levelPhysics.push_back((obj::CRigidBody*)pEntity);
                        }
                        else if(attrib == 0 && value == 2)
                        {
                            pEntity = new obj::CAnimation;
                            mp_levelEntities.push_back(pEntity);
                            mp_levelPhysics.push_back((obj::CAnimation*)pEntity);
                        }

                        // Motion
                        else if(attrib == 1 && value == 0)
                        {
                            if(pEntity != NULL) pEntity->SetStatic(false);
                        }

                        // Light source
                        else if(attrib == 2 && value == 1)
                        {
                            pLight = new gfx::CLight;
                            pLight->Init(gfx::IC_POINT_LIGHT, m_Window);
                            mp_levelLights.push_back(pLight);
                        }

                        // Player spawn / enemy spawn / etc.
                        else if(attrib == 3 && value == 1)
                        {
                            is_pspawn = true;
                        }
                        else if(attrib == 4 && value == 1)
                        {
                            mp_levelESpawns.push_back(pEntity);
                        }
                    }

                    if(pEntity == NULL) pEntity = new obj::CEntity;
                }

                else if(line.find("location") != std::string::npos)
                {
                    // location=x,y
                    std::vector<std::string> splitLines = util::split(line, '=');
                    if(splitLines.size() != 2) return false;

                    splitLines = util::split(splitLines[1], ',');
                    float x = atof(splitLines[0].c_str());
                    float y = atof(splitLines[1].c_str());

                    if(is_pspawn)
                    {
                        mp_PlayerSpawn.x = x;
                        mp_PlayerSpawn.y = y;
                    }
                    if(pEntity != NULL) pEntity->Move(x, y);
                }
            }
        }
    }

    file.close();
    m_filename = filename;
    return true;
}
