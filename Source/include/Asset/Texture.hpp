/**
 * @file
 *  Asset/Texture.hpp - Defines the CTexture OpenGL texture wrapper class.
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

#ifndef IRON_CLAD__ASSETS__TEXTURE_HPP
#define IRON_CLAD__ASSETS__TEXTURE_HPP

#include "Utils/Utilities.hpp"
#include "Base/Types.hpp"
#include "Asset.hpp"

namespace ic
{
namespace asset
{
    /**
     * A basic OpenGL texture asset.
     *  This asset is loaded dynamically using the CAssetManager
     *  system. It contains an OpenGL texture ID and several
     *  texture parameters such as width and height, determined
     *  on loading.
     **/
    class IRONCLAD_API CTexture : public CAsset
    {
    public:
        ~CTexture();

        /**
         * Loads a texture from a file.
         * @param   char*   Texture filename
         * @return  TRUE if loaded,
         *          FALSE on error.
         **/
        bool LoadFromFile(const char* pfilename);
        bool LoadFromFile(const std::string& filename);
        bool LoadFromRaw(const int iformat, const int format,
                         const int w, const int h,
                         const unsigned char* buffer);

        /**
         * Loads a Texture from an existing OpenGL texture handle.
         * @param   uint32_t    OpenGL texture handle
         * @return  Always TRUE, unless given texture <= 0.
         **/
        bool LoadFromTexture(const uint32_t texture);

        /**
         * Binds the texture to the OpenGL state for use.
         **/
        inline void Bind()
        { glBindTexture(GL_TEXTURE_2D, m_texture); }

        /**
         * Unbinds the texture from OpenGL.
         **/
        inline void Unbind()
        { glBindTexture(GL_TEXTURE_2D, 0); }

        inline uint32_t GetTextureID()
        { return m_texture; }

        inline uint32_t GetW()
        { return m_width; }

        inline uint32_t GetH()
        { return m_height; }

        /**
         * Only the CAssetManager class can create CTexture instances.
         **/
        friend class CAssetManager;

    private:
        CTexture(bool orig=false) : CAsset(orig), m_width(0), m_height(0),
            m_texture(0) {}
        CTexture(const CTexture& Copy);
        CTexture& operator= (const CTexture& Copy);

        void Release();

        uint32_t m_texture;
        int m_width, m_height;        
    };

}   // namespace asset
}   // namespace ic

#endif // IRON_CLAD__ASSETS__TEXTURE_HPP

/** @} **/
