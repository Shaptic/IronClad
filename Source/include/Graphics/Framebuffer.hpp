/**
 * @file
 *    Graphics/Framebuffer.hpp - Declares the CFrameBuffer class, 
 *    a wrapper for OpenGL FBOs.
 *
 * @author      George Kudrayvtsev (switch1440)
 * @version     1.0
 * @copyright   Apache License v2.0
 *  Licensed under the Apache License, Version 2.0 (the "License").            \n
 *  You may not use this file except in compliance with the License.        \n
 *  You may obtain a copy of the License at:
 *  http://www.apache.org/licenses/LICENSE-2.0                              \n
 *  Unless required by applicable law or agreed to in writing, software     \n
 *  distributed under the License is distributed on an "AS IS" BASIS,       \n
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\n
 *  See the License for the specific language governing permissions and     \n
 *  limitations under the License.
 *
 * @addtogroup Graphics
 * @{
 **/

#ifndef IRON_CLAD__GRAPHICS__FRAME_BUFFER_HPP
#define IRON_CLAD__GRAPHICS__FRAME_BUFFER_HPP

#include "Window.hpp"

namespace ic
{
namespace gfx
{
    /**
     * A wrapper for the OpenGL frame-buffer object.
     *  Frame-buffers are useful for rendering things off-screen, and
     *  are used in the CScene class to implement post-processing effects
     *  and deferred lighting using  render-to-texture.
     **/
    class IRONCLAD_API CFrameBuffer
    {
    public:
        CFrameBuffer() : m_texture(0), m_db(0), m_fbo(0) {}
        ~CFrameBuffer();

        /**
         * Initializes the various components of the frame-buffer.
         *  A depth buffer and color attachment are added to the
         *  frame-buffer on initialization.
         *  
         * @param   int     Width of  requested frame buffer
         * @param   int     Height of requested frame buffer
         * 
         * @return  TRUE if frame-buffer and components were created
         *          and attached successfully, 
         *          FALSE on any error.
         **/
        bool Init(const uint16_t width, const uint16_t height);

        /**
         * Clears the frame-buffer.
         **/
        void Clear();

        /**
         * Enables this frame-buffer.
         *  All subsequent rendering operations will happen onto this 
         *  framebuffer, until Disable() is called.
         **/
        void Enable();

        /**
         * Resumes rendering on-screen.
         **/
        void Disable();

        /**
         * Retrieves the contents of the frame-buffer.
         *  This can be used, of course, if you want to draw onto the 
         *  screen what you've drawn onto the frame-buffer.
         *
         * @return  Frame-buffer texture.
         **/
        inline uint32_t GetTexture() const
        { return m_texture; }

    private:
        math::vector2_t m_View, m_ThisView;
        uint32_t m_fbo, m_db;
        uint32_t m_texture;
    };
}   // namespace gfx
}   // namespace ic

#endif // IRON_CLAD__GRAPHICS__FRAME_BUFFER_HPP

/** @} **/