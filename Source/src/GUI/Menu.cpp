#include "IronClad/GUI/Menu.hpp"

using namespace ic;
using gui::CMenu;
using gui::CButton;

CMenu::CMenu(gfx::CScene& Scene) : m_Scene(Scene), mp_HoverSound(NULL),
    mp_ClickSound(NULL), mp_Background(NULL)
{
    mp_allButtons.clear();
}

CMenu::~CMenu()
{
    for(size_t i = 0; i < mp_allButtons.size(); ++i)
    {
        delete mp_allButtons[i];
        mp_allButtons[i] = NULL;
    }

    mp_allButtons.clear();
}

int16_t CMenu::AddButton(const char* texture_fn, const char* text,
                         const math::rect_t& Dimensions,
                         const char* htexture_fn /*= NULL*/,
                         const char* ctexture_fn /*= NULL*/,
                         gui::CFont* pFont       /*= NULL*/ )
{
    if(texture_fn ==  NULL || text == NULL) return -1;
    if(!m_Font.IsLoaded() && pFont == NULL) return -1;

    CButton* pButton = new CButton;
    
    pButton->LoadFont((pFont == NULL) ? m_Font : *pFont);

    // Parse the texture filename and attempt to create hover/click names.
    std::stringstream ss1, ss2;

    if(htexture_fn == NULL)
    {
        std::vector<std::string> splitUp = util::split(texture_fn, '.');
        ss1 << splitUp[0] << "Hover." << splitUp[1];
    }
    else
    {
        ss1 << htexture_fn;
    }

    if(ctexture_fn == NULL)
    {
        std::vector<std::string> splitUp = util::split(texture_fn, '.');
        ss2 << splitUp[0] << "Click." << splitUp[1];
    }
    else
    {
        ss2 << ctexture_fn;
    }

    pButton->Create(texture_fn, Dimensions, text, m_Scene,
                    ss1.str().c_str(), ss2.str().c_str());
    m_Scene.AddMesh(&pButton->GetEntity());
    
    mp_allButtons.push_back(pButton);
    return mp_allButtons.size();
}

int16_t CMenu::HandleEvent(const util::SystemEvent& Evt)
{
    if(mp_ClickSound != NULL && mp_ClickSound->GetAudioState() != AL_PLAYING)
        mp_ClickSound->UnloadSource();
    if(mp_HoverSound != NULL && mp_HoverSound->GetAudioState() != AL_PLAYING)
        mp_HoverSound->UnloadSource();

    switch(Evt.type)
    {
    case ic::util::IC_MOUSEDOWN:
        if(Evt.mbutton == ic::util::IC_BUTTON_LEFT)
        {
            ic::math::rect_t Box(Evt.mpos.x, Evt.mpos.y, 1, 1);

            for(size_t i = 0; i < mp_allButtons.size(); ++i)
            {
                if(mp_allButtons[i]->GetEntity().CheckCollision(Box))
                {
                    mp_allButtons[i]->SwitchToClick();
                }
            }
        }
        break;

    case ic::util::IC_MOUSEUP: 
        if(Evt.mbutton == ic::util::IC_BUTTON_LEFT)
        {
            ic::math::rect_t Box(Evt.mpos.x, Evt.mpos.y, 1, 1);

            for(size_t i = 0; i < mp_allButtons.size(); ++i)
            {
                if(mp_allButtons[i]->GetEntity().CheckCollision(Box))
                {
                    if(mp_ClickSound != NULL)
                    {
                        if(!mp_ClickSound->Play())
                        {
                            mp_ClickSound->UnloadSource();
                            mp_ClickSound->Play();
                        }
                    }
                    mp_allButtons[i]->SwitchToHover();
                    return i + 1;
                }
            }
        }
        break;

    case ic::util::IC_MOUSEMOVE:
    {
        ic::math::rect_t Box(Evt.mpos.x, Evt.mpos.y, 1, 1);

        for(size_t i = 0; i < mp_allButtons.size(); ++i)
        {
            if(mp_allButtons[i]->GetEntity().CheckCollision(Box))
            {
                if(mp_allButtons[i]->GetActive() != 1 &&
                   mp_HoverSound != NULL)
                {
                    if(!mp_HoverSound->Play())
                    {
                        mp_HoverSound->UnloadSource();
                        mp_HoverSound->Play();
                    }
                }
                mp_allButtons[i]->SwitchToHover();
            }
            else
            {
                mp_allButtons[i]->SwitchToMain();
            }
        }
        break;
    }
    }

    return -1;
}

bool ic::gui::CMenu::SetFont(const char* filename, const uint16_t size)
{
    return m_Font.LoadFromFile(filename, size);
}

void CMenu::SetFontColor(const float r, const float g, const float b)
{
    m_Font.SetFontColor(r, g, b);
}

void CMenu::SetFontColor(const color3f_t& Color)
{
    m_Font.SetFontColor(Color.r, Color.g, Color.b);
}

bool CMenu::SetFontSize(const uint16_t size)
{
    return m_Font.Resize(size);
}
