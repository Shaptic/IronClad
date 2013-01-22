/**
 * @file
 *    Math/Shapes.hpp - Defines various shapes, primarily used for collision.
 *
 * @author      George Kudrayvtsev (switch1440)
 * @version     1.1
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

#ifndef IRON_CLAD__MATH__SHAPES_HPP
#define IRON_CLAD__MATH__SHAPES_HPP

#include "MathDef.hpp"

namespace ic
{
namespace math
{
    struct IRONCLAD_API rect_t
    {
        rect_t(float x, float y, int w, int h) : x(x), y(y), w(w), h(h) {}
        rect_t() : x(0.f), y(0.f), w(0), h(0) {}

        /**
         * Tests collision with another rectangle.
         * @param   rect_t& Other rectangle
         * @return  TRUE if collision, FALSE if not
         **/
        inline bool Collides(const rect_t& Other) const
        {
            float top2, bottom2, right2, left2;

            left2   = Other.x;
            right2  = Other.x + Other.w;
            top2    = Other.y;
            bottom2 = Other.y + Other.h;

            if(y + h < top2)
                return false;
            if(y > bottom2)
                return false;
            if(x + w < left2)
                return false;
            if(x > right2)
                return false;

            return true;
        }
        inline bool Collides(const vector2_t& Point) const
        { return this->Collides(rect_t(Point.x, Point.y, 1, 1)); }

        float x, y;
        int   w, h;
    };

    struct IRONCLAD_API circle_t
    {
        float x, y, r;
    };
}
}

#endif // IRON_CLAD__MATH__SHAPES_HPP

/** @} **/