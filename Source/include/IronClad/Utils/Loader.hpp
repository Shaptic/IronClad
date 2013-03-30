/**
 * @file
 *    Utils/Loader.hpp - Defines many functions that simplify loading of
 *    files such as images.
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
 * @addtogroup Utilities
 * @{
 **/

#ifndef IRON_CLAD__UTILS__LOADER_HPP
#define IRON_CLAD__UTILS__LOADER_HPP

#include "GL/glew.h"
#include "GL/glfw.h"

#include "IronClad/Base/Types.hpp"

namespace ic
{
namespace util
{
    /**
     * Loads an image using the SDL_image API.
     *  The caller is responsible for cleaning up the returned surface.
     *  If the image is found to have an alpha component, the return value
     *  comes from load_image_alpha instead.
     *  Pixel data is directly accessible like so:
     *  SDL_Surface* pSurf = ic::util::load_image("Test.png");
     *  void* ppixels = pSurf->pixels;
     * 
     * @param   char*   Image path/filename
     * @param   int     Format to load image as (optional=GL_RGB)
     * 
     * @return  An SDL_Surface* containing data about the image, or
     *          NULL if the image failed to load.
     **/
    //SDL_Surface* load_image(const char* pfilename);
    //SDL_Surface* load_image_alpha(const char* pfilename);

    /**
     * Loads an image into GPU memory.
     *  This function uses the GLFW API to load the image located at
     *  the given filename. It is then loaded into GPU memory as a texture,
     *  and the texture handle is returned.
     *  If the image is found to have an alpha component, the returned 
     *  texture handle is a product of load_texture_alpha instead.
     *  Only .tga files are supported!
     *  
     * @param   char*   Image filename/path.
     * @param   int     Format to load the image into (optional=GL_RGB)
     * 
     * @return  The texture handle on successful loading,
     *          0 (zero) on failure.
     **/
    IRONCLAD_API uint32_t load_texture(const char* pfilename);
    IRONCLAD_API uint32_t load_texture_alpha(const char* pfilename);

    /**
     * Creates an SDL_Surface with the specified dimensions.
     *  Though the IronClad engine relies solely on OpenGL for hardware
     *  accelerated rendering, sometimes the surface manipulation
     *  techniques that SDL provides can come in handy.
     *  To render these surfaces, though, it is necessary to load them into
     *  GPU memory as OpenGL textures. This is easily done with a call to
     *  surface_to_texture.
     *
     * @param   vector2_t&  Requested surface dimensions.
     * @return  The created surface, or NULL on failure.
     **/
    //SDL_Surface* create_surface(math::vector2_t& Dim);
    //SDL_Surface* create_surface_alpha(math::vector2_t& Dim);

    /**
     * Converts an SDL surface to an OpenGL texture and loads it to GPU.
     *  Potential ways this function could fail include an unsupported 
     *  pixel format, or a lack of memory to create the texture.
     *  Errors can usually be figured out with a call to glGetError or
     *  SDL_GetError, depending on why the error occurred.
     *  
     * @param   SDL_Surface*    The surface to convert
     * @return  The OpenGL texture handle on successful conversion, or
     *          0 (zero) on failure.
     **/
    //uint32_t surface_to_texture(SDL_Surface* pSrc);

}   // namespace util
}   // namespace ic

#endif // IRON_CLAD__UTILS__LOADER_HPP

/** @} **/