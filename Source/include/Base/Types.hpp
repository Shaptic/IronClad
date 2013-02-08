/**
* @file
*  Base/Types.hpp - Declarations and definitions of various engine types.
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
* @addtogroup Engine
* @{
**/

#ifndef IRON_CLAD__BASE__TYPES_HPP
#define IRON_CLAD__BASE__TYPES_HPP

#include "Math/Math.hpp"

using namespace ic::math;

/**
 * A namespace encompassing the entire IronClad engine.
 */
namespace ic
{
    /**
     * Represents a 24-bit color using floating point numbers.
     *  Limited to the range [0, 1].
     **/
    struct IRONCLAD_API color3f_t
    {
        color3f_t() : r(0), g(0), b(0) {}
        color3f_t(float r, float g, float b) : r(r), g(g), b(b) {}
        float r, g, b;
    };

    /**
     * uint8_t version of the above (RGB).
     **/
    struct IRONCLAD_API color3i_t
    {
        color3i_t() : r(0), g(0), b(0) {}
        color3i_t(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
        uint8_t r, g, b;
    };

    /**
     * Represents a 32-bit floating point color (RGBA).
     *  Limited to the range [0, 1].
     **/
    struct IRONCLAD_API color4f_t
    {
        color4f_t() : r(1), g(1), b(1), a(1) {}
        color4f_t(float r, float g, float b, float a) : 
            r(r), g(g), b(b), a(a) {}
        float r, g, b, a;
    };

    /**
     * uint8_t version of the above (RGBA).
     **/
    struct IRONCLAD_API color4i_t
    {
        color4i_t() : r(1), g(1), b(1), a(1) {}
        color4i_t(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : 
            r(r), g(g), b(b), a(a) {}
        uint8_t r, g, b, a;
    };

    /**
     * Represents a vertex in 2 dimensions in the graphics pipeline.
     *  Contains a position and a color. Can be transformed using
     *  a matrix as well.
     **/
    struct IRONCLAD_API vertex2_t
    {
        vertex2_t() : Color(1.f, 1.f, 1.f, 1.f) {}
        vertex2_t(const vertex2_t& Copy)
        {
            Position = Copy.Position;
            TexCoord = Copy.TexCoord;
            Color    = Copy.Color;   
        }

        vector2_t   Position;
        vector2_t   TexCoord;
        color4f_t   Color;
    };

    /**
     * Represents a vertex in 3 dimensions in the graphics pipeline.
     *  Contains a position, a normal vector, and a color.
     *  Can be transformed using a matrix as well.
     **/
    struct IRONCLAD_API vertex3_t
    {
        vector3_t   Position;
        vector3_t   Normal;
        vector2_t   TexCoord;
        color4f_t   Color;
    };
    
    /**
     * The available lighting types.
     *  All lights support pulsing and casting shadows.
     **/
    enum IRONCLAD_API LightType
    {
        FAUX_LIGHT = -1,
        POINT_LIGHT,
        DIRECTIONAL_LIGHT,
        AMBIENT_LIGHT,
        SPOT_LIGHT,
        LIGHT_COUNT
    };
}   // namespace ic

#endif // IRON_CLAD__BASE__TYPES_HPP

/** @} **/
