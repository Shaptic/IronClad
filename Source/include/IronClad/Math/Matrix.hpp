/**
 * @file
 *  Math/Matrix.hpp - Declarations for the multi-dimensional matrix objects.
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

#ifndef IRON_CLAD__MATH__MATRIX_HPP
#define IRON_CLAD__MATH__MATRIX_HPP

#include <cmath>
#include <memory>
#include <iostream>

#include "IronClad/Math/MathDef.hpp"
#include "IronClad/Math/Vector2.hpp"
#include "IronClad/Math/Vector3.hpp"

namespace ic
{
namespace math
{
    /**
     * 4x4 matrix.
     *  There is support for rotation, translation, scaling,
     *  projection, and other functions critical to manipulation
     *  of vertices in OpenGL.
     **/
    class IRONCLAD_API matrix4x4_t
    {
    public:
        matrix4x4_t();
        matrix4x4_t(const float values[4][4]);
        matrix4x4_t(const float** ppvalues);
        matrix4x4_t(const matrix4x4_t& Copy);
        
        matrix4x4_t& operator=(const matrix4x4_t& Copy);

        /**
         * Provides the matrix's elements at that row.
         *  See the following examples:
         *  matrix[0][3] -> 3rd element of row 0 (r0, c3)
         *  matrix[3][1] -> 1st element of row 3 (r3, c1)
         *  matrix[2][1] -> 1st element of row 2 (r2, c1)
         *  
         * @param   uint8_t     Index
         * @return  The float array at that index.
         **/
        float* operator[](uint8_t index);

        matrix4x4_t operator*(matrix4x4_t& Other)   const;
        vector2_t   operator*(vector2_t& Other)     const;

        void Print() const;

        inline const float* GetMatrixPointer() const
        { return reinterpret_cast<const float*>(m_values); }

        /**
         * Creates a 2D projection matrix.
         * 
         * @param   uint32_t    Projection width
         * @param   uint32_t    Projection height
         * @param   int         Maximum projection depth
         * @param   int         Minimum projection depth
         *
         * @return  The requested projection matrix.
         **/
        static inline matrix4x4_t Projection2D(
            const uint32_t width, const uint32_t height,
            const int max_z,      const int min_z)
        {
            matrix4x4_t result;

            result[0][0] =  2.0f / width;
            result[0][3] = -1.0f;
            result[1][1] = -2.0f / height;
            result[1][3] =  1.0f;
            result[2][2] = -2.0f / (min_z - max_z);
            result[3][3] =  1.0f;

            return result;
        }

        static matrix4x4_t CreateIdentityMatrix();

    private:
        float m_values[4][4];
    };

    static const matrix4x4_t IDENTITY = 
        matrix4x4_t::CreateIdentityMatrix();

}   // namespace math
}   // namespace ic

#endif // IRON_CLAD__MATH__MATRIX_HPP

/** @} **/
