/**
 * @file
 *  Asset/Asset.hpp - Defines the CAsset abstract base class.
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
 * Contains all assets that the IronClad engine can load.
 *  Currently this includes textures, audio files, and meshes. It will be
 *  further expanded to include individual shader objects (not the vs/fs
 *  paired wrapper).
 *  
 * @{
 **/

#ifndef IRON_CLAD__ASSETS__ASSET_HPP
#define IRON_CLAD__ASSETS__ASSET_HPP

#include <string>

#include "Base/Types.hpp"

namespace ic
{

/**
 * Contains all asset types loadable by the IronClad engine.
 **/
namespace asset
{
    /**
     * The base class for all in-game assets.
     *  Throughout the IronClad engine, the only assets used
     *  will be textures and fonts. Assets contain a unique
     *  ID generated on creation that can provide fast access
     *  to it in the CAssetManager. The alternative is comparing
     *  filenames to find the respective asset, which can be
     *  much slower. Assets know if they are the "original"
     *  asset, or simply a copy using data from another asset.
     *  This allows deletion of the asset data to only occur
     *  on destruction if m_original = TRUE.
     *  The CAssetManager class takes care of figuring this out.
     **/
    class IRONCLAD_API CAsset
    {
    public:
        CAsset(bool original = false) : m_original(original),
            m_id(Hash((char*)this, sizeof(CAsset))) {}
        ~CAsset(){}

        virtual bool LoadFromFile(const char* pfilename) = 0;
        virtual bool LoadFromFile(const std::string& filename) = 0;

        inline const std::string& GetFilename() const
        { return m_filename; }

        inline uint32_t GetID() const
        { return m_id; }

        /**
         * Calculates a hash for a given value.
         * 
         * @param   char*       Data to hash
         * @param   uint32_t    Size of the data
         * 
         * @return  The hash value.
         **/
        static inline uint32_t Hash(const char* pdata, uint32_t size)
        {
            uint32_t hash = 0;
            uint32_t c = 0;

            while(size > 0)
            {
                c = *pdata++;
                hash = ((hash << 5) + hash) ^ c;
                --size;
            }

            return hash;
        }

        /**
         * Only the CAssetManager class can create CAsset instances.
         **/
        friend class CAssetManager;

    protected:
        /**
         * This should free the current asset if and only if it is
         * the original (m_original == true).
         **/
        virtual void Release() = 0;

        std::string m_filename;
        uint32_t    m_id;
        bool        m_original;
    };

}   // namespace asset
}   // namespace ic

#endif // IRON_CLAD__ASSETS__ASSET_HPP

/** @} **/
