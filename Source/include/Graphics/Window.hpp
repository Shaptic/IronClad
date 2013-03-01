/**
 * @file
 *  Graphics/Window.hpp - OpenGL window class for the IronClad engine.
 *
 * @author      George Kudrayvtsev (switch1440)
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
 * @addtogroup Graphics
 * Contains a majority of the low-level graphical abstraction.
 *  This group normally interfaces directly with OpenGL, or is a thin
 *  layer above another group that interfaces with it. The user should
 *  hardly ever need to see any of these classes.
 *  
 * @{
 **/

#ifndef IRON_CLAD__GRAPHICS__WINDOW_HPP
#define IRON_CLAD__GRAPHICS__WINDOW_HPP

// Needed for uint16_t (unsigned short).
#include <cstdint>

// OpenGL stuff
#include "GL/glew.h"
#include "GL/glfw.h"

#include "Utils/Utilities.hpp"
#include "Base/Errors.hpp"
#include "Base/Types.hpp"

namespace ic
{

/**
 * Contains most of the stuff related to drawing on the screen.
 **/
namespace gfx
{
    typedef bool (APIENTRY *PFNWGLSWAPINTERVALFARPROC)(int);
    static PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;

    /**
     * The minimum OpenGL version to use the IronClad engine.
     **/
    static const float IRONCLAD_MINIMUM_GL_VERSION = 3.3f;

    /**
     * An OpenGL compatible window.
     *  IronClad uses GLFW for window management, which supports contexts
     *  up to and including OpenGL version 4.2.
     *  The features requires by IronClad demand a minimum context version
     *  of 3.3, due to the use of VAOs (vertex array objects) and GLSL
     *  version 3.3.
     *  As the documentation may not be consistently updated, the
     *  IRONCLAD_MINIMUM_GL_VERSION constant will always contain the
     *  earliest supported GL version by IronClad.
     * 
     * @see IRONCLAD_MINIMUM_GL_VERSION
     **/
    class IRONCLAD_API CWindow
    {
    public:
        CWindow();
        ~CWindow();

        /**
         * Creates a window.
         *  Using the SDL library, this method creates an OpenGL context 
         *  with the specified attributes. If the _DEBUG macro is defined,
         *  the window is created in windowed mode, otherwise a full-screen
         *  window is created instead.
         *
         * @param   uint16_t    Window width    (optional=800)
         * @param   uint16_t    Window height   (optional=600)
         * @param   char*       Window title    (optional="OpenGL Window")
         *
         * @return TRUE on successful creation, FALSE on error.
         **/
        bool Create(
                const uint16_t  width   = 800,
                const uint16_t  height  = 600,
                const char*     ptitle  = "OpenGL Window");

        /**
         * Toggles full-screen mode.
         *  All OpenGL-related data (buffers, textures, etc.) MUST be 
         *  reloaded after calling this, because a new OpenGL context
         *  is created, invalidating all previous handles.
         *
         * @return TRUE if switching to FS mode, FALSE otherwise.
         **/
        bool ToggleFullscreen();

        /**
         * Resizes the window, and the OpenGL view-port.
         *  Calling this method with a modified width and height
         *  parameter will destroy the current context and create a
         *  new one, so everything OpenGL-related will need to be
         *  loaded once more. The new view-port matrix can be accessed
         *  with CWindow::QueryViewportMatrix(matrix4x4_t& mat).
         *
         * @param   uint16_t    New window width
         * @param   uint16_t    New window height
         *
         * @return  TRUE on successful resizing, FALSE on error.
         **/
        bool Resize(const uint16_t w, const uint16_t h);

        /**
         * Clears screen to a background color, clears color / depth bits.
         * @param   color4f_t   Background color (optional=0,0,0,1)
         **/
        void Clear(const color4f_t* BGColor = NULL);

        /**
         * Swaps the OpenGL buffers, rendering everything on-screen.
         **/
        void Update();

        inline uint16_t GetW() const
        { return m_width; }

        inline uint16_t GetH() const
        { return m_height; }

        inline bool GetFullscreen() const
        { return m_fullscreen; }

        static const float* GetProjectionMatrix()
        { return m_ProjectionMatrix.GetMatrixPointer(); }

        static const math::matrix4x4_t& GetProjectionMatrixC()
        { return m_ProjectionMatrix; }

        static void ToggleVSYNC();

    private:
        static math::matrix4x4_t   m_ProjectionMatrix;

        uint16_t    m_width, m_height;
        bool        m_fullscreen;
    };
}   // namespace gfx
}   // namespace ic

#endif IRON_CLAD__GRAPHICS__WINDOW_HPP

/** @}  */
