#include "Asset/AssetManager.hpp"

using namespace ic;
using asset::CAssetManager;

// Defines the static asset vector.
std::vector<asset::CAsset*> CAssetManager::s_allAssets;

CAssetManager::CAssetManager()
{
    CAssetManager::s_allAssets.clear();
}

CAssetManager::~CAssetManager()
{
    this->DestroyAll();
}

CAssetManager& CAssetManager::GetInstance()
{
    static CAssetManager Instance;
    return Instance;
}

asset::CAsset* CAssetManager::Find(const char* pfilename)
{
    // Comparing hashes is much faster than comparing strings.
    uint32_t searcher = asset::CAsset::Hash(pfilename, strlen(pfilename));

    for(size_t i = 0; i < CAssetManager::s_allAssets.size(); ++i)
    {
        const std::string& fn = CAssetManager::s_allAssets[i]->GetFilename();

        uint32_t result = asset::CAsset::Hash(fn.c_str(), fn.size());
        if(result == searcher)
            return CAssetManager::s_allAssets[i];
    }

    return NULL;
}

asset::CAsset* CAssetManager::Find(const std::string& filename)
{
    return CAssetManager::Find(filename.c_str());
}

asset::CAsset* CAssetManager::Find(const uint32_t id)
{
    // Searches for unique asset ID as opposed to 
    // by filename in the above example.
    for(size_t i = 0; i < CAssetManager::s_allAssets.size(); ++i)
    {
        if(CAssetManager::s_allAssets[i]->GetID() == id)
            return CAssetManager::s_allAssets[i];
    }

    return NULL;
}

void CAssetManager::DestroyAll()
{
    for(size_t i = 0; i < s_allAssets.size(); ++i)
    {
        CAssetManager::s_allAssets[i]->Release();
        delete CAssetManager::s_allAssets[i];
        CAssetManager::s_allAssets[i] = NULL;
    }

    CAssetManager::s_allAssets.clear();
}