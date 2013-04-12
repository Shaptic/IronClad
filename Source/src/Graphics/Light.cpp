#include "IronClad/Graphics/Light.hpp"

using namespace ic;
using gfx::CLight;

bool CLight::Init(const gfx::LightType type, const gfx::CWindow& Window)
{
    bool success = false;

    switch(type)
    {
    case IC_POINT_LIGHT:
        success = m_Shader.LoadFromFile("Shaders/Default.vs",
            "Shaders/PointLight.fs");
        break;

    case IC_DIRECTIONAL_LIGHT:
        success = m_Shader.LoadFromFile("Shaders/Default.vs",
            "Shaders/DirectionalLight.fs");
        break;

    case IC_AMBIENT_LIGHT:
        success = m_Shader.LoadFromFile("Shaders/Default.vs",
            "Shaders/AmbientLight.fs");
        break;

    case IC_NO_LIGHT: default: return false;
    }

    m_brtloc    = m_Shader.GetUniformLocation("light_brt");
    m_colloc    = m_Shader.GetUniformLocation("light_col");
    m_posloc    = m_Shader.GetUniformLocation("light_pos");
    m_attloc    = m_Shader.GetUniformLocation("light_att");
    m_maxloc    = m_Shader.GetUniformLocation("light_max");
    m_minloc    = m_Shader.GetUniformLocation("light_min");

    int scrloc  = m_Shader.GetUniformLocation("scr_height");
    int mvloc   = m_Shader.GetUniformLocation("mv");
    int projloc = m_Shader.GetUniformLocation("proj");

    m_Shader.Bind();
    glUniform1i(scrloc, Window.GetH());
    glUniformMatrix4fv(mvloc, 1, GL_TRUE, math::IDENTITY.GetMatrixPointer());
    glUniformMatrix4fv(projloc, 1, GL_TRUE, Window.GetProjectionMatrix());
    m_Shader.Unbind();

    m_type = type;

    return success;
}

bool CLight::Init(const gfx::LightType type, const uint16_t h, 
                  const math::matrix4x4_t& Proj)
{
    bool success = false;

    switch(type)
    {
    case IC_POINT_LIGHT:
        success = m_Shader.LoadFromFile("Shaders/Default.vs",
            "Shaders/PointLight.fs");
        break;

    case IC_DIRECTIONAL_LIGHT:
        success = m_Shader.LoadFromFile("Shaders/Default.vs",
            "Shaders/DirectionalLight.fs");
        break;

    case IC_AMBIENT_LIGHT:
        success = m_Shader.LoadFromFile("Shaders/Default.vs",
            "Shaders/AmbientLight.fs");
        break;

    case IC_NO_LIGHT: default: return false;
    }

    m_brtloc    = m_Shader.GetUniformLocation("light_brt");
    m_colloc    = m_Shader.GetUniformLocation("light_col");
    m_posloc    = m_Shader.GetUniformLocation("light_pos");
    m_attloc    = m_Shader.GetUniformLocation("light_att");
    m_maxloc    = m_Shader.GetUniformLocation("light_max");
    m_minloc    = m_Shader.GetUniformLocation("light_min");

    int scrloc  = m_Shader.GetUniformLocation("scr_height");
    int mvloc   = m_Shader.GetUniformLocation("mv");
    int projloc = m_Shader.GetUniformLocation("proj");

    m_Shader.Bind();
    if(scrloc != -1) glUniform1i(scrloc, h);
    glUniformMatrix4fv(mvloc, 1, GL_TRUE, math::IDENTITY.GetMatrixPointer());
    glUniformMatrix4fv(projloc, 1, GL_TRUE, Proj.GetMatrixPointer());
    m_Shader.Unbind();

    m_type = type;

    return success;
}

void CLight::SetBrightness(const float value)
{
    m_brt = value;
    glUniform1f(m_brtloc, value);
}

void CLight::SetColor(const float r, const float g, const float b)
{
    m_Color.r = r;
    m_Color.g = g;
    m_Color.b = b;

    glUniform3f(m_colloc, r, g, b);
}

void CLight::SetColor(const color3f_t& Color)
{
    this->SetColor(Color.r, Color.g, Color.b);
}

void CLight::SetAttenuation(const float c, const float l, const float q)
{
    m_Att.x = c;
    m_Att.y = l;
    m_Att.z = q;

    glUniform3f(m_attloc, c, l, q);
}

void CLight::SetAttenuation(const math::vector3_t& Att)
{
    this->SetAttenuation(Att.x, Att.y, Att.z);
}

void CLight::SetPosition(const float x, const float y)
{
    // For some reason, the position y value is off by 20 pixels,
    // so it is necessary to adjust for that.
    m_Pos = math::vector2_t(x, y);// + 20.f);

    glUniform2f(m_posloc, m_Pos.x, m_Pos.y);
}

void CLight::SetPosition(const math::vector2_t& Pos)
{
    this->SetPosition(Pos.x, Pos.y);
}

void CLight::Enable()
{
    m_Shader.Bind();
}

void CLight::Disable()
{
    m_Shader.Unbind();
}

void CLight::SetMaximumAngle(const float degrees)
{
    m_Max = math::vector2_t(1, 0);
    m_Max.Rotate(math::rad(degrees));

    glUniform2f(m_maxloc, m_Max.x, m_Max.y);
}

void CLight::SetMinimumAngle(const float degrees)
{
    m_Min = math::vector2_t(1, 0);
    m_Min.Rotate(math::rad(degrees));

    glUniform2f(m_minloc, m_Min.x, m_Min.y);
}
