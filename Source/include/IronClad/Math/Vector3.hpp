/**
 * @file
 *  Math/Vector3.hpp - Declarations for the 3D vector class.
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

#ifndef IRON_CLAD__MATH__VECTOR_3_HPP
#define IRON_CLAD__MATH__VECTOR_3_HPP

#include <cmath>

#include "IronClad/Math/MathDef.hpp"

namespace ic
{
namespace math
{
    /**
     * Represents a point in 3D rectangular-coordinate space.
     *  Supports all vector operations such as cross products,
     *  dot products, movement, scaling, and rotation.
     **/
    struct IRONCLAD_API vector3_t
    {
        /**
         * The vector components.
         **/
        float x, y, z;

        /**
         * Default constructor that sets components to 0.
         **/
        vector3_t() : x(0), y(0), z(0) {}

        /**
         * Constructs a 3D vector from components.
         * @param float X-component
         * @param float Y-component
         * @param float Z-component
         **/
        vector3_t(float x, float y, float z) : x(x), y(y), z(z) {}

        /**
         * Copy constructor.
         * @param vector3_t& The vector whose components will be copied.
         **/
        vector3_t(vector3_t& Copy) : x(Copy.x), y(Copy.y), z(Copy.z) {}

        /**
         * Assignment operator.
         * @param vector3_t& The vector whose components will be copied.
         **/
        inline vector3_t& operator=(const vector3_t& Copy)
        { x = Copy.x; y = Copy.y; z = Copy.z; return (*this); }

        // Begin mathematical vector operations.

        /**
         * Calculates the cross product of two vectors.
         * @param   vector3_t& The other vector
         * @return  A 3D vector normal to both vectors.
         **/
        inline vector3_t operator^(const vector3_t& Other) const;

        /**
         * Accesses the vector elements in a different way.
         * @param   int Element index
         * @return  x or y element based on index.
         **/
        inline float operator[](const int index) const
        { return index == 0 ? x : index == 1 ? y : index == 3 ? z : x; }

        /**
         * Calculates the dot product of two vectors.
         * @param   vector3_t& The other vector
         * @return  The dot product as a scalar.
         **/
        inline float operator*(const vector3_t& Other) const
        { return Other.x * x + Other.y * y + Other.z * z; }

        /**
         * Multiplies the current vector by a scalar factor.
         * @param   float The scalar
         * @return  A 3D resultant vector.
         **/
        inline vector3_t operator*(const float scalar) const
        { return vector3_t(x * scalar, y * scalar, z * scalar); }

        /**
         * Divides the current vector by a scalar factor.
         * @param   float The scalar
         * @return  A 3D resultant vector.
         **/
        inline vector3_t operator/(const float scalar) const
        { return vector3_t(x / scalar, y / scalar, z / scalar); }

        /**
         * Adds a given vector to the current vector, returning the result.
         * @param   vector3_t& The other vector
         * @return  A 3D resultant vector.
         **/
        inline vector3_t operator+(const vector3_t& Other) const
        { return vector3_t(x + Other.x, y + Other.y, z + Other.z); }

        /**
         * Subtracts a given vector from the current vector, returning the result.
         * @param   vector3_t& The other vector
         * @return  A 3D resultant vector.
         **/
        inline vector3_t operator-(const vector3_t& Other) const
        { return vector3_t(x - Other.x, y - Other.y, z - Other.z); }

        /**
         * Normalizes the current vector.
         *  This *DOES* modify the current vector, and makes it into
         *  a unit vector version of itself.
         **/
        void Normalize()
        {
            float mag = this->Magnitude();
            x /= mag;
            y /= mag;
            z /= mag;
        }

        /**
         * Calculates the current vector's magnitude.
         * @return The magnitude.
         **/
        inline float Magnitude() const
        { return sqrt(x * x + y * y + z * z); }

        /**
         * Returns the normalized version of the current vector,
         * without actually modifying it.
         * @see vector3_t::Normalize()
         **/
        vector3_t GetNormalized() const
        {
            float mag = this->Magnitude();
            return vector3_t(x / mag, y / mag, z / mag);
        }
    };

}   // namespace math
}   // namespace ic

#endif // IRON_CLAD__MATH__VECTOR_3_HPP

/** @} **/
