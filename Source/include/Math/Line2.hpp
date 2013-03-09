/**
 * @file
 *    Math/Line2.hpp - Describes a 2-dimensional line segment.
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
 * @addtogroup Math
 * @{
 **/

#ifndef IRON_CLAD__MATH__LINE2_HPP
#define IRON_CLAD__MATH__LINE2_HPP

#include <cstddef>

#include "Vector2.hpp"

namespace ic
{
namespace math
{
    /**
     * A line-segment.
     *  Yes, I lied. It's not really a line, going out to infinity forever
     *  and ever. It's merely a segment, a portion of a line. It's defined
     *  by a start and end point, represented by vector2_t's. You can 
     *  perform operations such as rotation and determine intersections
     *  on the line.
     **/
    struct line2_t
    {
    public:
        line2_t(){}
        line2_t(const float start_x, const float start_y, const float end_x, const float end_y);
        line2_t(const vector2_t& Start, const vector2_t& End);

        inline line2_t& operator=(const line2_t& Other)
        { Start = Other.Start; End = Other.End; return (*this); }

        /**
         * Calculates whether or not a line intersects with this one.
         *  End-points touching counts as intersection. The mathematical
         *  calculation to solve for the point of intersection is as
         *  follows:
         *    
         *      First line:             y - y1 = m1(x - x1)
         *      Second line:            y - y2 = m2(x - x2)
         *      Equal to each other:    m1x - m1x1 + y1 = m2x - m2x2 + y2
         *      Solve for 'x':          m1x - m2x = -m2x2 + y2 + m1x1 + y1
         *                              x(m1 - m2) = -m2x2 + y2 + m1x1 + y1
         *                              x = (-m2x2 + y2 + m1x1 + y1) / (m1 - m2)
         *      Solve for 'y':          y = m1(x) - m1x1 + y1
         *      Thus (x, y) becomes the intersection point.
         *
         *  Then, if the point (x, y) falls on both line segments, there
         *  is indeed a collision. Otherwise, there is not, naturally.
         *  Infinite slopes are taken care of and the user needn't worry
         *  about providing a vertical line. A link is provided that 
         *  contains a more detailed explanation for those interested.
         *    
         *  You can pass a vector2_t to the method if you would like to
         *  know the actual point of intersection.
         *
         * @param   line2_t     The other line
         * @param   vector2_t*  The point of intersection (optional=NULL)
         *
         * @return  TRUE if they intersect, FALSE otherwise.
         * @see http://gamedev.stackexchange.com/questions/26004/how-to-detect-2d-line-on-line-collision
         **/
        bool Intersects(const line2_t& Other, vector2_t* pIntersect) const;

        inline bool Intersects(const vector2_t& Point) const
        { return this->OnSegment(Point); }

        /**
         * Detects whether or not a point lies on the current line segment.
         * @param   vector2_t&  Point to check
         * @return  TRUE if it lies on the segment, FALSE if not.
         **/
        bool OnSegment(const vector2_t& Point) const;

        /**
         * Rotates the line a certain number of radians.
         *  This method really only calls vector2_t::Rotate on both the 
         *  start and end-points.
         *  The reason that radians is chosen as the type of angle 
         *  measure here is because conversion from degrees to radians can 
         *  end up being slightly costly when done a lot since it involves
         *  division, an expensive CPU operation. So, if a user is aware
         *  that they will be rotating often, they can convert a single
         *  time and pass it here often; 
         *  std::sin and std::cos expect radians.
         *    
         * @param   float   Angle in radians.
         **/
        void Rotate(const float radians);

        /**
         * Calculates the slope of the line.
         *  Slope is calculated via the basic formula 
         *      m = (y2 - y1) / (x2 - x1)
         *  If there is an infinite slope, the optional flag will be
         *  set and the returned slope will be 0.
         *    
         * @param   bool*   Optional flag to track infinite slope (optional=NULL)
         * 
         * @return  Slope, 0 if infinite.
         **/
        float GetSlope(bool* is_inf = NULL) const;

        /**
         * Calculates the length of the line segment.
         *  This is done via the Pythagorean theorem
         *  d = sqrt(x*x + y*y)
         *        
         *  If the user prefers to avoid the square-root operation, known
         *  to be slow, they can pass a flag.
         *    
         * @param   bool    Perform square root?    (optional=TRUE)
         * 
         * @return  Length of line segment.
         **/
        inline float GetLength(const bool do_sqrt = true) const
        {
            return math::distance(Start.x, Start.y, End.x, End.y, do_sqrt);
        }

        vector2_t    Start;
        vector2_t    End;
    };

}    // namespace math
}    // namespace ic

#endif // IRON_CLAD__MATH__LINE2_HPP

/** @} **/
