#include "Graphics/Effect.hpp"

using namespace ic;
using gfx::CEffect;

bool CEffect::Init(const gfx::EffectType type)
{
    switch(type)
    {
    case IC_VERTICAL_GAUSSIAN_BLUR:
        return m_Effect.LoadFromFile("Shaders/Default.vs",
            "Shaders/GaussianBlurV.fs");

    case IC_HORIZONTAL_GAUSSIAN_BLUR:
        return m_Effect.LoadFromFile("Shaders/Default.vs",
            "Shaders/GaussianBlurH.fs");

    case IC_GRAYSCALE:
        return m_Effect.LoadFromFile("Shaders/Default.vs",
            "Shaders/FontRender.fs");

    case IC_FADE:
        return m_Effect.LoadFromFile("Shaders/Default.vs",
            "Shaders/Fade.fs");

    case IC_NO_EFFECT: 
        return m_Effect.LoadFromFile("Shaders/Default.vs",
            "Shaders/Default.fs");

    default: return false;
    }
}

bool CEffect::SetParameter(const char* pname, const float* pvalues,
    const uint32_t size)
{
    int loc = this->GetLocation(pname);

    switch(size)
    {
    case 2: glUniform2fv(loc, 1, pvalues); break;
    case 3: glUniform3fv(loc, 1, pvalues); break;
    case 4: glUniform4fv(loc, 1, pvalues); break;
    default: return false;
    }

    return (loc != -1);
}

bool CEffect::SetParameter(const char* pname, const int* pvalues,
    const uint32_t size)
{
    int loc = this->GetLocation(pname);

    switch(size)
    {
    case 2: glUniform2iv(loc, 1, pvalues); break;
    case 3: glUniform3iv(loc, 1, pvalues); break;
    case 4: glUniform4iv(loc, 1, pvalues); break;
    default: return false;
    }

    return (loc != -1);
}

bool CEffect::SetParameter(const char* pname, const float value)
{
    int loc = this->GetLocation(pname);
    glUniform1f(loc, value);
    return (loc != -1);
}

bool CEffect::SetParameter(const char* pname, const int value)
{
    int loc = this->GetLocation(pname);
    glUniform1i(loc, value);
    return (loc != -1);
}

bool CEffect::SetMatrix(const char* pname, const math::matrix4x4_t& Mat)
{
    int loc = this->GetLocation(pname);
    glUniformMatrix4fv(loc, 1, GL_TRUE, Mat.GetMatrixPointer());
    return (loc != -1);
}

bool CEffect::SetMatrix(const char* pname, const float* pvalues)
{
    int loc = this->GetLocation(pname);
    glUniformMatrix4fv(loc, 1, GL_TRUE, pvalues);

    return (loc != -1);
}

int CEffect::GetLocation(const char* pname)
{
    if(m_Effect.GetProgram() == 0) return -1;
    return m_Effect.GetUniformLocation(pname);    
    
    // Failed attempt at a uniform hash table is below.

    std::string name(pname);
    name += m_Effect.GetProgram();

    uint32_t searcher_hash = asset::CAsset::Hash(
        name.c_str(), name.length());

    std::map<uint32_t, uint32_t>::iterator i = 
        m_UniformHash.find(searcher_hash);

    if(i == m_UniformHash.end())
    {
        int result = m_Effect.GetUniformLocation(pname);
        if(result == -1) return -1;

        m_UniformHash[searcher_hash] = result;
    }
    else
    {
        return m_UniformHash[searcher_hash];
    }
}