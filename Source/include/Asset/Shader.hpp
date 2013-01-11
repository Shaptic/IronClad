/**
 * @file
 *    Asset/Shader.hpp - Individual OpenGL shader objects.
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
 * @addtogroup Assets
 * @{
 **/

#ifndef IRON_CLAD__ASSET__SHADER_HPP
#define IRON_CLAD__ASSET__SHADER_HPP

#include "GL/glew.h"
#include "GL/glfw.h"

#include "Asset.hpp"
#include "Utils/Utilities.hpp"

namespace ic
{
namespace asset
{
    /**
     * An individual OpenGL shader object.
     *  These are used in the ShaderPair class, which groups a fragment
     *  and vertex shader object into a single class and links them to
     *  a program.
     **/
    class IRONCLAD_API CShader : public CAsset
    {
    public:
        CShader(bool orig=false) : CAsset(orig), m_shader(0),
            m_error(GL_NO_ERROR), m_error_str("No error") {}
        ~CShader();

        bool LoadFromFile(const char* pfilename);
        bool LoadFromFile(const std::string& filename);

        inline uint32_t GetShaderObject() const
        { return m_shader; }

        inline const std::string& GetError() const
        { return m_error_str; }

    private:
        void Release();

        std::string m_error_str;
        uint32_t    m_shader;
        int         m_error;        
    };


}   // namespace asset
}   // namespace ic

#endif // IRON_CLAD__ASSET__SHADER_HPP

/** @} **/