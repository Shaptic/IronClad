/**
 * @file
 *	Button.hpp - Defines a button class for use in dynamic menus.
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

#ifndef IRON_CLAD__GUI_HPP
#define IRON_CLAD__GUI_HPP

#include "Math/Math.hpp"
#include "Entity/RigidBody.hpp"
#include "Graphics/Scene.hpp"
#include "Graphics/Globals.hpp"
#include "Font.hpp"

namespace ic
{
namespace gui
{
    enum Alignment
    {
        IC_ALIGN_CENTER,
        IC_ALIGN_LEFT,
        IC_ALIGN_RIGHT
    };

    class IRONCLAD_API CButton
    {
    public:
        CButton();
        ~CButton();

        /**
         * Loads a button texture of the specified dimensions, with text.
         *  This function will automatically calculate the location at
         *  which to render the text based on the alignment set via
         *  SetAlignment(), which defaults to IC_ALIGN_CENTER.
         *  We are rendering our button texture and text to a frame buffer,
         *  then assigning this as the texture override for the entity
         *  itself. That way, shader effects and such can be applied w/o
         *  the text rendering interfering, and physics can be done for
         *  collision with the mouse and whatnot.
         *  
         * @param   char*   Texture filename
         * @param   rect_t  Dimensions to resize the texture to
         * @param   char*   Text to render on the button
         * @param   CScene  Scene to load button geometry into
         * 
         * @return  TRUE if everything successful, FALSE if any errors.
         *          It will also return FALSE if the text is too tall / wide
         *          for the given dimensions.
         **/
        bool Create(const char* texture,
                    const math::rect_t& Dim,
                    const char* text,
                    gfx::CScene& Scene);

        bool LoadFont(CFont& pFont);

        inline CRigidBody& GetEntity()
        { return m_Entity; }

    private:
        gfx::CFrameBuffer   m_FBO;
        asset::CTexture*    mp_Texture;        
        math::rect_t        m_Dimensions;
        math::vector2_t     m_Pos;

        CFont               m_Font;
        CRigidBody          m_Entity;

        Alignment           m_align;
        std::string         m_text;

        int                 m_handle;
    };
}
}

#endif // IRON_CLAD__GUI_HPP

/** @} **/