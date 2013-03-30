/**
 * @file
 *  Asset/AssetManager.hpp - Defines the CAssetManager class, 
 *  responsible for loading and tracking CAsset instances.
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
 * @addtogroup Assets
 * @{
 **/

#ifndef IRON_CLAD__ASSETS__ASSET_MANAGER_HPP
#define IRON_CLAD__ASSETS__ASSET_MANAGER_HPP

#include <vector>

#include "IronClad/Utils/Utilities.hpp"
#include "Asset.hpp"

namespace ic
{
namespace asset
{
    /**
     * Loads, finds, creates, and manages assets.
     *  This class is a singleton, and should be globally
     *  accessible through ic::asset::g_AssetManager.
     *
     *  Only use the Find method if you are absolutely sure
     *  that the asset exists, otherwise use the Create method,
     *  which will create and load an asset if it does not exist
     *  or will return the existing one if it does.
     **/
    class IRONCLAD_API CAssetManager
    {
    public:
        virtual ~CAssetManager();
        static CAssetManager& GetInstance();

        /**
        * Searches for an asset with a matching filename.
        *  It is recommended that Find<T> *only* be used when
        *  you are absolutely sure that an asset already exists.
        *  The Create<T> method will ensure that an asset is not
        *  being loaded again and will always return a usable
        *  asset.
        *
        * @param   char*   Filename
        * @return  NULL if no asset exists, otherwise the asset.
        **/
        static CAsset* Find(const char* pfilename);
        static CAsset* Find(const std::string& filename);
        static CAsset* Find(const uint32_t asset_id);

        /**
         * Creates an asset, using previously loaded data if it exists.
         *  This method will log an error if the asset fails to load.
         *
         * @param   char*   Asset path/filename
         * @return  The loaded asset, or NULL if creation failed.
         **/
        template<typename T>
        static T* Create(const char* pfilename);

        template<typename T>
        static T* Create(const std::string& filename);

        template<typename T>
        static T* Create();

        /**
         * Requests destruction of an existing asset.
         * 
         * @param   CAsset*     Asset to destroy
         * 
         * @return  TRUE if asset exists, FALSE if not.
         **/
        template<typename T>
        static bool Destroy(T* pAsset);

        /**
         * Destorys all assets.
         *  This should only be called by engine cleanup.
         **/
        static void DestroyAll();

        static inline uint32_t GetAssetCount()
        { return CAssetManager::s_allAssets.size(); }

    private:
        CAssetManager();
        CAssetManager(const CAssetManager&);
        CAssetManager& operator=(const CAssetManager&);

        static std::vector<CAsset*> s_allAssets;
    };

    // Inserts the actual template definitions.
    #include "AssetManager.inl"

}   // namespace asset
}   // namespace ic

#endif // IRON_CLAD__ASSETS__MESH_HPP

/** @} **/
