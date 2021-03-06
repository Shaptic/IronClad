/**
 * @file
 *  GUI/Menu.hpp - A high level class for easily creating menus and handling events.
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
 * @addtogroup GUI
 * @{
 **/

#ifndef IRON_CLAD__GUI__MENU_HPP
#define IRON_CLAD__GUI__MENU_HPP

#include "IronClad/Asset/Sound2D.hpp"
#include "Button.hpp"

namespace ic
{
namespace gui
{
    /**
     * Allows for easy creation of menus with custom backgrounds,
     * buttons, and titles.
     **/
    class IRONCLAD_API CMenu
    {
    public:
        CMenu(gfx::CWindow& Window);
        ~CMenu();
        
        /**
         * Initializes the internal scene.
         **/
        bool Init();

        /**
         * Adds a button to the menu at a certain position.
         *  Because this is meant to be highly customizable, there are
         *  multiple optional arguments 
         *  For the most general case, you can call
         *      AddButton(filename, text, pos)
         *  and just be done with it. The function will automatically try
         *  to load the hover and click textures by appending Hover and
         *  Click to the filename, respectively. 
         *      Example: Given Button.tga, it will attempt to load
         *               ButtonClick.tga and ButtonHover.tga.
         *
         *  Otherwise, you can specify precisely what the texture names are.
         *  You can also specify a font to use, if, for some reason, 
         *  different buttons use different fonts. By default, the font
         *  given via SetFont() will be used.
         *  
         * @param   char*   Button texture filename
         * @param   char*   Text to render on the button
         * @param   rect_t  Dimensions and position of the button
         * @param   char*   Hover texture filename      (optional=NULL)
         * @param   char*   Click texture filename      (optional=NULL)
         * @param   CFont*  Custom font for this button (optional=NULL)
         * 
         * @return  A unique menu button ID if all went well, -1 if not.
         *          Possible causes for error include:
         *              No custom text and SetFont() not called,
         *              the textures failed to load, or
         *              the text was not rendered on the button properly.
         *
         * @pre     SetFont() must have been called successfully.
         * @post    The gfx::CScene given in the ctor is filled with data.
         **/
        int16_t AddButton(const char* texture_fn,
                          const char* text,
                          const math::rect_t& Dimension,
                          const char* htexture_fn = NULL,
                          const char* ctexture_fn = NULL,
                          gui::CFont* pFont       = NULL);

        /**
         * Handles system events for hover, click, etc.
         * @param   util::SystemEvent   Event to handle
         * @return  Menu button ID if it was clicked, -1 otherwise.
         **/
        int16_t HandleEvent(const util::SystemEvent& Evt);

        void Disable()
        {
            if(mp_Background != NULL) mp_Background->SetRendering(false);
            for(size_t i = 0; i < mp_allButtons.size(); ++i)
            {
                mp_allButtons[i]->GetEntity().SetRendering(false);
            }
        }

        void Enable()
        {
            if(mp_Background != NULL) mp_Background->SetRendering(true);
            for(size_t i = 0; i < mp_allButtons.size(); ++i)
            {
                mp_allButtons[i]->GetEntity().SetRendering(true);
            }
        }

        void Render()
        {
            m_Scene.Render();
        }

        inline void UpdateCursor()
        { if(mp_Cursor) mp_Cursor->Move(util::GetMousePosition()); }

        inline void UpdateWindow(gfx::CWindow& Win)
        { m_Scene.UpdateWindow(Win); }

        /**
         * Loads a font for use in title, buttons, etc.
         * @param   char*   Filepath to font
         * @return  TRUE on success, FALSE on error.
         **/
        bool SetFont(const char* filename, const uint16_t size);

        /**
         * Loads a sound to be player on hovering.
         * @param   char*   Filepath to sound effect
         * @return  TRUE on success, FALSE on error.
         **/
        bool SetHoverSound(const char* filename)
        {
            mp_HoverSound = (asset::CSound2D*)asset::CAssetManager
                                  ::Create<asset::CSound2D>(filename);
            if(mp_HoverSound == NULL)
            {
                return false;
            }
            else
            {
                mp_HoverSound->SetVolume(0.7f);
                return true;
            }
        }

        /**
         * Loads a sound to be player on button-press.
         * @param   char*   Filepath to sound effect
         * @return  TRUE on success, FALSE on error.
         **/
        bool SetClickSound(const char* filename)
        {
            mp_ClickSound = (asset::CSound2D*)asset::CAssetManager
                            ::Create<asset::CSound2D>(filename);
            if(mp_ClickSound == NULL)
            {
                return false;
            }
            else
            {
                mp_ClickSound->SetVolume(0.8f);
                return true;
            }
        }

        bool SetBackground(const std::string& filepath)
        {
            mp_Background = new obj::CEntity;
            if(!mp_Background->LoadFromImage(filepath.c_str(),
                                             m_Scene.GetGeometryBuffer()))
                return false;

            m_Scene.AddMesh(mp_Background);
            return true;
        }

        bool SetCursor(const std::string& img_name)
        {
            mp_Cursor = new obj::CEntity;
            if(!mp_Cursor->LoadFromImage(img_name.c_str(),
                                         m_Scene.GetGeometryBuffer()))
            {
                return false;
            }

            m_Scene.AddMesh(mp_Cursor);
            return true;
        }

        bool SetFontSize(const uint16_t size);
        void SetFontColor(const color3f_t& Color);
        void SetFontColor(const float r, const float g, const float b);
        //void SetTitle(const char* title, const math::vector2_t& Position);

        CButton* GetButton(const uint16_t id)
        { return (id >= mp_allButtons.size()) ? NULL : mp_allButtons[id]; }

        CButton* GetButton(const std::string& text)
        {
            for(size_t i = 0; i < mp_allButtons.size(); ++i)
                if(mp_allButtons[i] == text) return mp_allButtons[i];

            return NULL;
        }

        inline gfx::CScene& GetScene() { return m_Scene; }

    private:
        gfx::CScene         m_Scene;
        asset::CSound2D*    mp_HoverSound;
        asset::CSound2D*    mp_ClickSound;
        obj::CEntity*       mp_Background;
        obj::CEntity*       mp_Cursor;
        gui::CFont          m_Font;

        std::vector<CButton*> mp_allButtons;
    };
}
}

#endif // IRON_CLAD__GUI__MENU_HPP

/** @} **/