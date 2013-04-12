#include "IronClad/Level.hpp"

using namespace ic;
using util::g_Log;
using util::CParser;

CLevel::CLevel(const gfx::CWindow& Window) : m_Window(Window)
{
    mp_lvlOther.clear();
    mp_lvlLights.clear();
    mp_lvlAnimations.clear();
    mp_levelEntities.clear();
    m_lvlSpawns.clear();
    m_filename.clear();
}

CLevel::~CLevel()
{
    this->Clear<obj::CAnimation>(mp_lvlAnimations);
    this->Clear<obj::CRigidBody>(mp_lvlBodies);
    this->Clear<obj::CEntity>(mp_lvlOther);
    this->Clear<gfx::CLight>(mp_lvlLights);

    m_lvlSpawns.clear();
    m_filename.clear();
}

bool CLevel::LoadFromFile(const std::string& filename, gfx::CScene& Scene)
{
    std::stringstream ss;
    std::ifstream file;
    std::string line;

    file.open(filename);
    if(!file.is_open()) return false;

    CParser Parser;

    std::streampos current;

    while(std::getline(file, line) && (line.empty() || line != "</level>"))
    {
        if(line.empty() || line[0] == '/')
        {
            continue;
            current = file.tellg();
        }

        std::cout << line << std::endl;

        if(line.find("<entity>") != std::string::npos)
        {
            std::streampos ent_s = file.tellg();
            std::streampos ent_e = CParser::FindInFile(file, "</entity>");
            if(ent_e == std::streampos(-1))
            {
                g_Log.Flush();
                g_Log << "[ERROR] Failed to find closing tag for entity";
                g_Log << "in level: " << filename << "\n";
                g_Log.PrintLastLog();
                return false;
            }

            // Load other parameters (non-mesh).
            Parser.LoadFromStream(file, ent_s, ent_e);

            obj::CEntity* pEntity = NULL;

            if(Parser.GetValueb("isAnimation"))
            {
                std::string f = Parser.GetValue("animation");
                if(f.empty())
                {
                    g_Log.Flush();
                    g_Log << "[ERROR] Animation specified without file ";
                    g_Log << "in level: " << filename << "\n";
                    g_Log.PrintLastLog();
                    goto search;
                }

                pEntity = new obj::CAnimation;
                if(!pEntity->LoadFromFile(f, Scene.GetGeometryBuffer()))
                {
                    g_Log.Flush();
                    g_Log << "[ERROR] Failed to create animation from ";
                    g_Log << "level entity: " << filename << "\n";
                    g_Log.PrintLastLog();
                }
                else
                {
                    mp_lvlAnimations.push_back((obj::CAnimation*)pEntity);
                    mp_levelEntities.push_back(pEntity);
                    Scene.AddMesh(pEntity);
                }

                if(!Parser.GetValue("animationRate").empty())
                {
                    ((obj::CAnimation*)pEntity)->SetAnimationRate(
                        Parser.GetValuef("animationRate"));
                }
            }
            else
            {
                // Load a mesh in the current entity block.
                file.seekg(current);
                asset::CMesh* pMesh = asset::CAssetManager
                                           ::Create<asset::CMesh>();

                std::stringstream fn;
                fn << filename << ":Mesh";
                pMesh->SetFilename(fn.str());
                fn.str(std::string());

                if(!pMesh->LoadFromExisting(file, ent_e))
                {
                    g_Log.Flush();
                    g_Log << "[ERROR] Failed to load mesh from file stream: ";
                    g_Log << filename << "\n";
                    g_Log.PrintLastLog();
                }

                if(Parser.GetValueb("isPhysical"))
                {
                    pEntity = new obj::CRigidBody;

                    if(Parser.GetValueb("isStatic"))
                    {
                        ((obj::CRigidBody*)pEntity)->SetStatic(true);
                    }
                    
                    mp_lvlBodies.push_back((obj::CRigidBody*)pEntity);
                }
                else
                {
                    pEntity = new obj::CEntity;
                }

                if(!pEntity->LoadFromMesh(pMesh, Scene.GetGeometryBuffer()))
                {
                    g_Log.Flush();
                    g_Log << "[ERROR] Failed to create entity from level ";
                    g_Log << "mesh: " << filename << "\n";
                    g_Log.PrintLastLog();
                }
                else
                {
                    mp_levelEntities.push_back(pEntity);
                    Scene.AddMesh(pEntity);
                }
            }

            std::string line = Parser.GetValue("position");
            if(line.empty())
            {
                // Use x=, y= keys.
                pEntity->Move(Parser.GetValuef("x"), 
                    Parser.GetValuef("y"));
            }
            else
            {
                std::vector<std::string> p =
                    Parser.GetValues("position", ',');

                if(p.size() == 2)
                {
                    pEntity->Move(atof(p[0].c_str()),
                        atof(p[1].c_str()));
                }
                else
                {
                    g_Log.Flush();
                    g_Log << "[INFO] Malformed position specification ";
                    g_Log << "for entity in level: " << filename << "\n";
                    g_Log.PrintLastLog();
                }
            }

            file.seekg(ent_e);
            Parser.Reset();
        }

        else if(line.find("<light>") != std::string::npos)
        {
            std::streampos lt_s = file.tellg();
            std::streampos lt_e = CParser::FindInFile(file, "</light>");
            if(lt_e == std::streampos(-1))
            {
                g_Log.Flush();
                g_Log << "[ERROR] Failed to find closing tag for light ";
                g_Log << "in level: " << filename << "\n";
                g_Log.PrintLastLog();
                return false;
            }

            // Load light configuration.
            Parser.LoadFromStream(file, lt_s, lt_e);

            gfx::CLight* pLight = new gfx::CLight;

            gfx::LightType t = (gfx::LightType)Parser.GetValuei("type");
            pLight->Init(t, m_Window);
            pLight->Enable();

            // Brightness; all lights.
            pLight->SetBrightness(Parser.GetValuef("brightness"));

            // Color; all lights.
            std::vector<std::string> c = Parser.GetValues("color", ',');
            if(c.size() == 3)
            {
                pLight->SetColor(atof(c[0].c_str()),
                    atof(c[1].c_str()),
                    atof(c[2].c_str()));
            }
            else
            {
                g_Log.Flush();
                g_Log << "[INFO] Malformed color specification for light ";
                g_Log << "in level: " << filename << "\n";
                g_Log.PrintLastLog();

                pLight->SetColor(1, 1, 1);
            }

            // Attenuation; non-ambient lights only.
            if(t != gfx::IC_AMBIENT_LIGHT)
            {
                c = Parser.GetValues("attenuation", ',');
                if(c.size() == 3)
                {
                    pLight->SetAttenuation(atof(c[0].c_str()),
                                           atof(c[1].c_str()),
                                           atof(c[2].c_str()));
                }
                else
                {
                    g_Log.Flush();
                    g_Log << "[INFO] Malformed attenuation specification ";
                    g_Log << "for light in level: " << filename << "\n";
                    g_Log.PrintLastLog();
                }

                // Likewise for position.
                std::string line = Parser.GetValue("position");
                if(line.empty())
                {
                    // Use x=, y= keys.
                    pLight->SetPosition(Parser.GetValuef("x"), 
                                        Parser.GetValuef("y"));
                }
                else
                {
                    c = Parser.GetValues("position", ',');
                    if(c.size() == 2)
                    {
                        pLight->SetPosition(atof(c[0].c_str()),
                                            atof(c[1].c_str()));
                    }
                    else
                    {
                        g_Log.Flush();
                        g_Log << "[INFO] Malformed position specification ";
                        g_Log << "for light in level: " << filename << "\n";
                        g_Log.PrintLastLog();
                    }
                }
            }

            // Angles; directional lights only.
            if(t == gfx::IC_DIRECTIONAL_LIGHT)
            {
                pLight->SetMinimumAngle(Parser.GetValuef("minAngle"));
                pLight->SetMaximumAngle(Parser.GetValuef("maxAngle"));
            }

            pLight->Disable();
            Scene.AddLight(pLight);
            mp_lvlLights.push_back(pLight);
            Parser.Reset();
        }

search:
        current = file.tellg();
        continue;
    }

    file.close();
    m_filename = filename;
    return true;
}
