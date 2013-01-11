/**
 * @file
 *  Graphics/Material.hpp - Declarations for a CShader OpenGL shader wrapper.
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
 * @addtogroup Graphics
 * @{
 **/

#ifndef IRON_CLAD__GRAPHICS__SHADER_HPP
#define IRON_CLAD__GRAPHICS__SHADER_HPP

#include <string>

#include "Base/Types.hpp"
#include "Asset/Shader.hpp"
#include "Asset/AssetManager.hpp"

namespace ic
{
namespace gfx
{
    /**
     * An OpenGL shader wrapper class.
     *  This class will load both a vertex and fragment shader, linking
     *  them together into a single program. It provides an easy interface
     *  for retrieving attribute/uniform locations.
     **/
    class IRONCLAD_API CShaderPair
    {
    public:
        CShaderPair();
        ~CShaderPair();

        CShaderPair& operator=(const CShaderPair& Copy);

        /**
         * Loads a pair of shader files.
         *  Error strings can be retrieved by calling GetError().
         * 
         * @param   char*   Vertex shader path/filename
         * @param   char*   Fragment shader path/filename
         * 
         * @return  TRUE on successful loading of both shaders,
         *          FALSE on error.
         **/
        bool LoadFromFile(const char* pvs_filename,
            const char* pfs_filename);

        bool LoadFromFile(const std::string& vs_filename,
            const std::string& fs_filename);

        /**
         * Loads a shader file from source code.
         *  On returning false, an error string is stored internally, which
         *  can be accessed by calling GetError().
         *  
         * @param   char**  Vertex shader source
         * @param   char**  Fragment shader source
         * 
         * @return  TRUE on successful loading of both shaders,
         *          FALSE on error.
         **/
        bool LoadFromSource(const char** pvs_src, const char** pfs_src);

        void Bind();
        void Unbind();

        const std::string& GetError() const;

        inline uint32_t GetProgram() const
        { return m_program; }

        short   GetUniformLocation(const char* uni)     const;
        short   GetAttributeLocation(const char* attr)  const;

    private:
        asset::CShader* mp_VShader;
        asset::CShader* mp_FShader;

        std::string m_error_str;

        uint32_t m_program;
        
        int     m_error;
    };

}   // namespace gfx
}   // namespace ic

#endif // IRON_CLAD__GRAPHICS__SHADER_HPP

/** @} **/
