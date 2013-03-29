/**
 * @file
 *    Graphics/Light.hpp - Declares the CLight class, which handles scene lighting in IronClad.
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
 * @addtogroup Graphics
 * @{
 **/

#ifndef IRON_CLAD__GRAPHICS__LIGHT_HPP
#define IRON_CLAD__GRAPHICS__LIGHT_HPP

#include "Base/Types.hpp"
#include "Window.hpp"
#include "ShaderPair.hpp"

namespace ic
{
namespace gfx
{
    /**
     * Various types of lights.
     **/
    IRONCLAD_API enum LightType
    {
        IC_NO_LIGHT             = -1,
        IC_POINT_LIGHT,
        IC_DIRECTIONAL_LIGHT,
        IC_AMBIENT_LIGHT,
        IC_LIGHT_TYPE_COUNT
    };

    /**
     * Represents a light within a scene in IronClad.
     *  Lights are normally created and set up separately from a scene,
     *  but can be accessed within one also. There are multiple types of
     *  lights, defined in the LightType enumeration. Each light type
     *  has its own special variables and parameters that can be
     *  customized.
     **/
    class IRONCLAD_API CLight
    {
    public:
        CLight() : m_brt(.5f), m_Att(.05f, .01f, 0.f),
                   m_type(IC_NO_LIGHT) {}
        ~CLight(){}

        bool Init(const LightType type, const CWindow& Window);
        
        void SetBrightness(const float brt);
        void SetAngle(const float degrees);

        void SetColor(const float r, const float g, const float b);
        void SetAttenuation(const float c, const float l, const float q);
        void SetPosition(const float x, const float y);

        void SetColor(const color3f_t& Color);
        void SetAttenuation(const math::vector3_t& Att);
        void SetPosition(const math::vector2_t& Pos);

        void SetMaximumAngle(const float degrees);
        void SetMinimumAngle(const float degrees);

        void Enable();
        void Disable();

        const color3f_t&        GetColor() const        { return m_Color; }
        const math::vector3_t&  GetAttenuation() const  { return m_Att;   }
        const math::vector2_t&  GetPosition() const     { return m_Pos;   }
        float                   GetBrightness() const   { return m_brt;   }

        LightType               GetType() const         { return m_type; }

    private:
        CShaderPair     m_Shader;

        color3f_t       m_Color;
        math::vector3_t m_Att;
        math::vector2_t m_Pos;
        math::vector2_t m_Max, m_Min;

        LightType       m_type;

        float           m_brt;

        // Uniform locations.
        int m_brtloc, m_colloc, m_attloc, m_posloc;
        int m_maxloc, m_minloc;
    };

    typedef std::vector<ic::gfx::CLight*> LightVector;

}   // namespace gfx
}   // namespace ic

#endif // IRON_CLAD__GRAPHICS__LIGHT_HPP

/** @} **/
