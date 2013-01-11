/**
 * @file
 *  Math/Vector2.hpp - Declarations for the 2D vector class.
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

#ifndef IRON_CLAD__MATH__VECTOR_2_HPP
#define IRON_CLAD__MATH__VECTOR_2_HPP

#include <cmath>
#include <iostream>

#include "Math/MathDef.hpp"

namespace ic
{
namespace math
{
    /**
     * Forward declaration of a 3D vector for use in the
     * 2D vector cross product calculation.
     **/
    struct IRONCLAD_API vector3_t;

    /**
     * Forward declaration of a 4x4 matrix for use in
     * 2D vector translation.
     **/
    class IRONCLAD_API matrix4x4_t;

    /**
     * Represents a point in 2D rectangular-coordinate space.
     *  Supports all vector operations such as cross products,
     *  dot products, movement, scaling, and rotation.
     *
     * @todo Implement the cross product method.
     * @todo Add support for translation via matrices.
     **/
    struct IRONCLAD_API vector2_t
    {
        /**
         * The vector components.
         **/
        float x, y;

        /**
         * Default constructor that sets components to 0.
         **/
        vector2_t() : x(0), y(0) {}

        /**
         * Constructs a 2D vector from components.
         * @param float X-component
         * @param float Y-component
         **/
        vector2_t(float x, float y) : x(x), y(y) {}

        /**
         * Copy constructor.
         * @param vector2_t& The vector whose components will be copied
         **/
        vector2_t(const vector2_t& Copy) : x(Copy.x), y(Copy.y) {}

        /**
         * Assignment operator.
         * @param vector2_t& The vector whose components will be copied
         **/
        inline vector2_t& operator=(const vector2_t& Copy)
        { x = Copy.x; y = Copy.y; return (*this); }

        /**
         * Comparison operator. 
         * @param   vector2_t&  Vector to compare with
         * @return  TRUE if vectors are equal, FALSE if not.
         * @warning Floating-point comparisons are inaccurate.
         **/
        inline bool operator!=(const vector2_t& Other) const
        { return !(x == Other.x && y == Other.y); }

        /**
         * Comparison operator. 
         * @param   vector2_t&  Vector to compare with
         * @return  FALSE if vectors are equal, TRUE if not.
         **/
        inline bool operator==(const vector2_t& Other) const
        { return (x != Other.x || y != Other.y); }

        // Begin mathematical vector operations.

        /**
         * Calculates the cross product of two vectors.
         * @param   vector2_t& The other vector
         * @return  A 3D vector normal to both vectors.
         **/
        inline vector3_t operator^(const vector2_t& Other) const;

        /**
         * Accesses the vector elements in a different way.
         * @param   int Element index
         * @return  x or y element based on index.
         **/
        inline float operator[](const int index) const
        { return (index % 2 == 0 ? x : y); }

        /**
         * Calculates the dot product of two vectors.
         * @param   vector2_t& The other vector
         * @return  The dot product as a scalar.
         **/
        inline float operator*(const vector2_t& Other) const
        { return Other.x * x + Other.y * y; }

        /**
         * Multiplies the current vector by a scalar factor.
         * @param   float The scalar
         * @return  A 2D resultant vector.
         **/
        inline vector2_t operator*(const float scalar) const
        { return vector2_t(x * scalar, y * scalar); }

        /**
         * Divides the current vector by a scalar factor.
         * @param   float The scalar
         * @return  A 2D resultant vector.
         **/
        inline vector2_t operator/(const float scalar) const
        { return vector2_t(x / scalar, y / scalar); }

        /**
         * Adds a given vector to the current vector, returning the result.
         * @param   vector2_t& The other vector
         * @return  A 2D resultant vector.
         **/
        inline vector2_t operator+(const vector2_t& Other) const
        { return vector2_t(x + Other.x, y + Other.y); }

        /**
         * Adds a value to both components of the current vector.
         * @param   float   The value to add
         * @return  A 2D resultant vector.
         **/
        inline vector2_t operator+(const float value) const
        { return vector2_t(x + value, y + value); }


        /**
         * Subtracts a given vector from the current vector, returning the result.
         * @param   vector2_t& The other vector
         * @return  A 2D resultant vector.
         **/
        inline vector2_t operator-(const vector2_t& Other) const
        { return vector2_t(x - Other.x, y - Other.y); }

        /**
         * Normalizes the current vector.
         *  This *DOES* modify the current vector, and makes it into
         *  a unit vector version of itself.
         **/
        inline void Normalize()
        {
            float mag = this->Magnitude();
            x /= mag;
            y /= mag;
        }

        /**
         * Calculates the current vector's magnitude.
         * @return The magnitude.
         **/
        inline float Magnitude() const
        { return sqrt(x * x + y * y); }

        /**
         * Rotates the current vector using the rotation matrix.
         *  The rotation matrix (in right-hand Cartesian plane)
         *  is defined as being
         *  | x | | cos(d), -sin(d) |
         *  | y | | sin(d),  cos(d) |
         *
         *  But in the OpenGL coordinate system, the origin is in
         *  the top-left, as opposed to bottom-left, as shown above.
         *  So rotations are actually inverted and the matrix is
         *  | x | | cos(d),  sin(d) |
         *  | y | | -sin(d), cos(d) |
         *
         *  So the final rotation in OpenGL would be:
         *      x =  x * cos(d) + y * sin(d)
         *      y = -x * sin(d) + y * cos(d)
         *
         * @param float The rotation angle in radians.
         **/
        inline void Rotate(const float radians)
        {
            x =  x * cos(radians) + y * sin(radians);
            y = -x * sin(radians) + y * cos(radians);
        }

        /**
         * Translates the current vector by a matrix.
         * @param   matrix4x4_t&    Translation matrix
         **/
        void Translate(matrix4x4_t& trans_mat);

        /**
         * Outputs the vector data to stdout in the format <x, y>.
         **/
        inline void Print() const
        {
            std::cout.setf(std::ios::fixed, std::ios::floatfield);
            std::cout.precision(2);
            std::cout << "<" << x << ", " << y << ">" << std::endl;
        }

        /**
         * Returns the normalized version of the current vector,
         * without actually modifying it.
         * @see vector2_t::Normalize()
         **/
        inline vector2_t GetNormalized() const
        {
            float mag = this->Magnitude();
            return vector2_t(x / mag, y / mag);
        }
    };

}   // namespace math
}   // namespace ic

#endif // IRON_CLAD__MATH__VECTOR_2_HPP

/** @} **/
