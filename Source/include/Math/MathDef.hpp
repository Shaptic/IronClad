/**
 * @file
 *  Math/MathDef.hpp - Declarations and definitions of various math types.
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
 * @addtogroup Math
 * @{
 **/

#ifndef IRON_CLAD__MATH__MATH_DEF_HPP
#define IRON_CLAD__MATH__MATH_DEF_HPP

#include <cmath>
#include <cstdint>

/**
 * Defines a DLL exporting macro to simplify things.
 *  You may wonder why this is placed here. Well, every header needs this
 *  defined, and this file includes nothing, and everything includes
 *  this, so it works out nicely.
 **/
#ifdef IRONCLAD_EXPORTS
  #define IRONCLAD_API __declspec(dllexport)
#else
  #ifdef _DEBUG
    #define IRONCLAD_API
  #else
    #define IRONCLAD_API __declspec(dllimport)
  #endif // _DEBUG
#endif  // IRONCLAD_EXPORTS

namespace ic
{

/**
 * Contains all math-related functionality.
 **/
namespace math
{
    /**
     * Mathematical constant.
     **/
    static const float PI = 3.14159265f;

    /**
     * Easily convert degrees to radians.
     * @param   float   Angle in degrees
     * @return  Angle in radians.
     **/
    inline float rad(const float theta)
    {
        return theta * PI / 180.0f;
    }

    /**
     * Easily convert radians to degrees.
     * @param   float   Angle in radians
     * @return  Angle in degrees.
     **/
    inline float deg(const float theta)
    {
        return theta * 180.0f / PI;
    }

    /**
     * Calculates the distance between two points.
     *  Since the square root is an expensive operation, it is optional.
     *  
     * @param   float   Starting x-coordinate
     * @param   float   Starting y-coordinate
     * @param   float   Ending x-coordinate
     * @param   float   Ending y-coordinate
     * @param   bool    Do square root operation?   (optional=true)
     **/
    inline float distance(
        const float start_x, const float start_y,
        const float end_x, const float end_y,
        const bool do_sqrt = true)
    {
        float d =   (end_x - start_x) * (end_x - start_x) + 
                    (end_y - start_y) * (end_y - start_y);

        return do_sqrt ? sqrt(d) : d;
    }

    /**
     * Clamps a particular value to a certain range.
     * 
     * @param   T&  Numeric type for the value to clamp
     * @param   T   Numeric type for min value
     * @param   T   Numeric type for max value
     **/
    template<typename T>
    void clamp(T& val, T minimum, T maximum)
    {
        if(val > maximum)       val = maximum;
        else if(val < minimum)  val = minimum;
    }

}   // namespace math
}   // namespace ic

#endif // IRON_CLAD__MATH__MATH_DEF_HPP

/** @} **/
