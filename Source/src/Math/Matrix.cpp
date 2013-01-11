#include "Math/Matrix.hpp"

using namespace ic;
using math::matrix4x4_t;

matrix4x4_t::matrix4x4_t()
{
    memset(m_values, 0, sizeof m_values);
}

matrix4x4_t::matrix4x4_t(const float values[4][4])
{
    for(size_t i = 0; i < 4; ++i)
        for(size_t j = 0; j < 4; ++j)
            m_values[i][j] = values[i][j];
}

matrix4x4_t::matrix4x4_t(const float** ppvalues)
{
    for(size_t i = 0; i < 4; ++i)
        for(size_t j = 0; j < 4; ++j)
            m_values[i][j] = ppvalues[i][j];
}

matrix4x4_t::matrix4x4_t(const matrix4x4_t& Copy)
{
    memcpy(m_values, Copy.GetMatrixPointer(), sizeof m_values);
}

matrix4x4_t& matrix4x4_t::operator=(const matrix4x4_t& Copy)
{
    memcpy(m_values, Copy.GetMatrixPointer(), sizeof m_values);

    return (*this);
}

float* matrix4x4_t::operator[](uint8_t index)
{
    math::clamp<uint8_t>(index, 0, 3);
    return m_values[index];
}

matrix4x4_t matrix4x4_t::operator*(matrix4x4_t& Other) const
{
    matrix4x4_t Res;

    for(uint8_t r = 0; r < 4; ++r)
        for(uint8_t c = 0; c < 4; ++c)
            for(uint8_t e = 0; e < 4; ++e)
                Res[r][c] += (m_values[r][e]) * (Other[e][c]);

    return Res;
}

math::vector2_t matrix4x4_t::operator*(math::vector2_t& Other) const
{
    vector2_t Result;
    
    Result.x = Other.x * m_values[0][0] + Other.y * m_values[0][1];
    Result.x += m_values[0][2] + m_values[0][3];

    Result.y = Other.x * m_values[1][0] + Other.y * m_values[1][1];
    Result.y += m_values[1][2] + m_values[1][3];

    return Result;
}

void matrix4x4_t::Print() const
{
    for(size_t r = 0; r < 4; ++r)
    {
        std::cout << "| ";

        for(size_t c = 0; c < 4; ++c)
        {
            std::cout <<  m_values[r][c] << " ";
        }

        std::cout << "|\n";
    }
}

matrix4x4_t matrix4x4_t::CreateIdentityMatrix()
{
    matrix4x4_t Result;
    Result[0][0] = Result[1][1] = Result[2][2] = Result[3][3] = 1.0f;
    return Result;
}