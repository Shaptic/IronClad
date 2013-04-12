/**
 * @file
 *  Graphics/Effect.hpp - Declaration of the abstract base class for all of
 *  the effects supported by IronClad.
 *
 * @author      George Kudrayvtsev (halcyon)
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
 * @addtogroup Graphics
 * @{
 **/

#ifndef IRON_CLAD__GRAPHICS__EFFECT_HPP
#define IRON_CLAD__GRAPHICS__EFFECT_HPP

#include <map>

#include "ShaderPair.hpp"

namespace ic
{
namespace gfx
{
    /**
     * All of the effects supported by IronClad's shader bundle.
     *  These types simply determine which file to load into the material.
     **/
    IRONCLAD_API enum EffectType
    {
        IC_NO_EFFECT = -1,
        IC_HORIZONTAL_GAUSSIAN_BLUR,
        IC_VERTICAL_GAUSSIAN_BLUR,
        IC_GRAYSCALE,
        IC_FADE,
        IC_RIPPLE,
        IC_EFFECT_COUNT
    };

    class IRONCLAD_API CEffect
    {
    public:
        CEffect();
        ~CEffect(){}

        bool Init(const EffectType type);

        /**
         * Sets an effect parameter.
         *  The parameter name *MUST* match the variable name in the
         *  effect's respective shader.
         * 
         * @param   char*       Parameter name
         * @param   T           Parameter type (floats, integers, etc)
         * @param   uint32_t    Size of the parameter array
         * 
         * @return  TRUE if the parameter exists, FALSE if not.
         **/
        bool SetParameter(const char* pname, const float* pvalues, const uint32_t size);
        bool SetParameter(const char* pname, const int*   pvalues, const uint32_t size);
        bool SetParameter(const char* pname, const float  value);        
        bool SetParameter(const char* pname, const int    value);

        /**
         * Sends a 4x4 matrix to the shader.
         * 
         * @param   char*               Name of matrix variable
         * @param   math::matrix4x4_t   Matrix to send
         * @return  TRUE if pname exists in shader, FALSE if not.
         **/
        bool SetMatrix(const char* pname, const math::matrix4x4_t& Mat);
        bool SetMatrix(const char* pname, const float* pvalues);

        /**
         * Starts using this effect.
         **/
        inline void Enable()
        { m_Effect.Bind(); }
    
        /**
         * Stops using this effect.
         **/
        inline void Disable()
        { m_Effect.Unbind(); }

    private:
        int GetLocation(const char* pvar);

        gfx::CShaderPair m_Effect;

        std::map<uint32_t, uint32_t> m_UniformHash;
    };

    class CFadeEffect
    {
    public:
        //void AddEntityToQueue(const obj::CEntity* pEntity)
        //{
        //    //mp_fadeQueue.push_back(pEntity);
        //    //
        //}

        void SetMode(bool in)
        {
            m_mode  = in;
            m_alpha = m_mode ? 0.f : 1.f;
        }

        void Reset()
        {
            m_alpha = m_mode ? 0.f : 1.f;
        }
        
        bool Fade(CEffect* pEffect, const float rate)
        {
            m_mode ? m_alpha += rate : m_alpha -= rate;
            math::clamp<float>(m_alpha, 0.f, 1.f);
            
            pEffect->Enable();
            pEffect->SetParameter("alpha", m_alpha);
            pEffect->Disable();

            if(m_mode) return m_alpha >= 1.f;
            else       return m_alpha <= 0.f;
        }

        float GetAlpha() const { return m_alpha; }

    private:
        bool m_mode;
        float m_alpha;
    };
}   // namespace gfx
}   // namespace ic

#endif // IRON_CLAD__GRAPHICS__EFFECT_HPP

/** @} **/
