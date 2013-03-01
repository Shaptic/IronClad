/**
 * @file
 *	GUI/Menu.hpp - A high level class for easily creating menus and handling events.
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
 * @addtogroup GUI
 * @{
 **/

#ifndef IRON_CLAD__GUI__MENU_HPP
#define IRON_CLAD__GUI__MENU_HPP

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
        CMenu(gfx::CScene& Scene);
        ~CMenu();

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
         * @return  Menu button ID if it was clicked, -1 otherwise.
         **/
        int16_t HandleEvent(const util::SystemEvent& Evt);

        /**
         * Loads a font for use in title, buttons, etc.
         * @param   char*   Filepath to font
         * @return  TRUE on success, FALSE on error.
         **/
        bool SetFont(const char* filename, const uint16_t size);

    private:
        gfx::CScene&    m_Scene;
        gui::CFont      m_Font;

        std::vector<CButton*> mp_allButtons;
    };
}
}

#endif // IRON_CLAD__GUI__MENU_HPP

/** @} **/