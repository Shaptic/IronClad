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
#include "Entity/Entity.hpp"
#include "Graphics/Scene.hpp"
#include "Graphics/Globals.hpp"
#include "Fonts/Font.hpp"

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

    class CButton
    {
    public:
        CButton() : m_align(IC_ALIGN_CENTER) {}
        ~CButton(){}

        /**
         * Loads a button texture of the specified dimensions, with text.
         *  This function will automatically calculate the location at
         *  which to render the text based on the alignment set via
         *  SetAlignment(), which defaults to IC_ALIGN_CENTER.
         *  
         * @param   char*   Texture filename
         * @param   rect_t  Dimensions to resize the texture to
         * @param   char*   Text to render on the button
         * 
         * @return  TRUE if everything successful, FALSE if any errors.
         *          It will also return FALSE if the text is too tall / wide
         *          for the given dimensions.
         **/
        bool Create(const char* texture,
                    const math::rect_t& Dim,
                    const char* text,
                    gfx::CScene& Scene)
        {
            gfx::CVertexBuffer VBO;
            gfx::CFrameBuffer  FBO;

            asset::CTexture* pTexture = asset::CAssetManager::Create<asset::CTexture>(texture);
            asset::CMesh*    pMesh    = asset::CAssetManager::Create<asset::CMesh>();

            vertex2_t quad_v[4];

            quad_v[0].Position = math::vector2_t(0, 0);
            quad_v[1].Position = math::vector2_t(Dim.w, 0.f);
            quad_v[2].Position = math::vector2_t(Dim.w, Dim.h);
            quad_v[3].Position = math::vector2_t(0.f,   Dim.h);
                
            /*
            quad_v[0].Position = math::vector2_t(Dim.x,         Dim.y);
            quad_v[1].Position = math::vector2_t(Dim.x + Dim.w, Dim.y);
            quad_v[2].Position = math::vector2_t(Dim.x + Dim.w, Dim.y + Dim.h);
            quad_v[3].Position = math::vector2_t(Dim.x,         Dim.y + Dim.h);*/

            quad_v[0].TexCoord = math::vector2_t(0, 0);
            quad_v[1].TexCoord = math::vector2_t(1, 0);
            quad_v[2].TexCoord = math::vector2_t(1, 1);
            quad_v[3].TexCoord = math::vector2_t(0, 1);

            float x_off = 0.f, y_off = 0.f;
            if(m_align == IC_ALIGN_CENTER)
            {
                x_off = Dim.w / 2.f - m_Font.GetTextWidth(text) / 2.f;
            }

            // Overflows, oops.
            uint32_t th = m_Font.GetTextHeight(text);
            int32_t h   = Dim.h;

            y_off = (h - th) / 2.f;

            m_Pos = math::vector2_t(Dim.x + x_off, Dim.y + Dim.h - y_off);

            if( pTexture == NULL || pMesh == NULL || 
               !pMesh->LoadFromRaw(quad_v, 4, gfx::Globals::g_FullscreenIndices, 6))
            {
                asset::CAssetManager::Destroy<asset::CTexture>(pTexture);
                asset::CAssetManager::Destroy<asset::CMesh>(pMesh);
                return false;
            }

            VBO.Init();
            FBO.Init(pTexture->GetW(), pTexture->GetH());

            m_Entity.LoadFromMesh(pMesh, VBO);//Scene.GetGeometryBuffer());
            m_Entity.SetMaterialOverride(pTexture);
            //Scene.AddMesh(&m_Entity);            
            
            FBO.Enable();
            gfx::Globals::g_DefaultEffect.Enable();
            gfx::Globals::g_DefaultEffect.SetMatrix("proj", gfx::CWindow::GetProjectionMatrix());
            gfx::Globals::g_DefaultEffect.SetMatrix("mv", math::IDENTITY);
            VBO.Draw();
            glBegin(GL_TRIANGLES);
            glColor4f(1.f, 0.f, 0, 1);
            glVertex2f(0.f, 10.f);
            glVertex2f(0.f, 20.f);
            glVertex2f(20.f, 10.f);
            glEnd();
            m_Font.RenderCached();
            gfx::Globals::g_DefaultEffect.Disable();
            FBO.Disable();

            m_handle = FBO.GetTexture();

            m_text = text;
            m_Font.CacheText(text, m_Pos);

            return true;
        }

        bool LoadFont(const font::CFont* pFont);
        bool LoadFont(const char* pfilename, const uint16_t size)
        {
            return m_Font.LoadFromFile(pfilename, size);
        }

        void Render()
        {
            //glBindTexture(GL_TEXTURE_2D, m_handle);
            //gfx::Globals::g_DefaultEffect.Enable();
            //gfx::Globals::g_DefaultEffect.SetMatrix("proj", gfx::CWindow::GetProjectionMatrix());
            //gfx::Globals::g_DefaultEffect.SetMatrix("mv", math::IDENTITY);
            //gfx::Globals::g_FullscreenVBO.Draw();
            //gfx::Globals::g_DefaultEffect.Disable();
            //glBindTexture(GL_TEXTURE_2D, 0);
            //m_Font.RenderCached();
        }

    private:
        font::CFont  m_Font;
        
        CEntity      m_Entity;
        
        math::rect_t    m_Dimensions;
        math::vector2_t m_Pos;

        Alignment    m_align;
        int m_handle;
        
        std::string  m_text;

    };
}
}

#endif // IRON_CLAD__GUI_HPP

/** @} **/