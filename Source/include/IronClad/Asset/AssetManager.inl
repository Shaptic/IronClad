#include "AssetManager.hpp"

using namespace ic;
using asset::CAssetManager;
using util::g_Log;

template<typename T>
T* CAssetManager::Create(const char* pfilename,
                         const void* powner)
{
    T* pFinder = (T*)CAssetManager::Find(pfilename, powner);

    // The asset does *not* have an original, load one.
    if(pFinder == NULL)
    {
        T* pAsset = new T(true, powner);

        if(pAsset->LoadFromFile(pfilename))
        {
            g_Log.Flush();
            g_Log << "[INFO] Loaded asset:      (";
            g_Log.SetWidth(10) << pAsset->GetID() << ") ";
            g_Log << pfilename << "\n";
            g_Log.PrintLastLog();

            CAssetManager::s_allAssets.push_back(pAsset);
            return (T*)CAssetManager::s_allAssets.back();
        }
        else
        {
            g_Log.Flush();
            g_Log << "[ERROR] Failed to load:   (";
            g_Log.SetWidth(10) << pAsset->GetID() << ") ";
            g_Log << pfilename << "\n";
            g_Log << "[ERROR] Log: " << pAsset->GetError() << "\n";
            g_Log.PrintLastLog();

            delete pAsset;
            return (pAsset = NULL);
        }
    }

    // The asset already exists, return it.
    g_Log.Flush();
    g_Log << "[INFO] Retrieved asset:   (";
    g_Log.SetWidth(10) << pFinder->GetID() << ") " << pfilename << "\n";
    g_Log.PrintLastLog();

    return pFinder;
}

template<typename T>
T* CAssetManager::Create(const std::string& filename,
                         const void* powner)
{
    return CAssetManager::Create<T>(filename.c_str(), powner);
}

template<typename T>
T* CAssetManager::Create(const void* powner)
{
    T* pResult = new T(true, powner);
    CAssetManager::s_allAssets.push_back(pResult);
    
    g_Log.Flush();
    g_Log << "[INFO] Created new asset: (";
    g_Log.SetWidth(10) << pResult->GetID() << ")\n";
    g_Log.PrintLastLog();

    return pResult;
}

template<typename T>
bool CAssetManager::Destroy(T* pAsset)
{
    if(pAsset == NULL) return false;

    for(size_t i = 0; i < CAssetManager::s_allAssets.size(); ++i)
    {
        if(s_allAssets[i] == pAsset)
        {
            // Log stuff.
            g_Log.Flush();
            g_Log << "[INFO] Deleting asset:    (";
            g_Log.SetWidth(10) << pAsset->GetID() << ") ";
            g_Log << pAsset->GetFilename() << "\n";
            g_Log.PrintLastLog();

            // Delete asset, destructor should handle resource de-alloc.
            delete pAsset;
            pAsset = NULL;

            // Remove from manager.
            CAssetManager::s_allAssets.erase(
                CAssetManager::s_allAssets.begin() + i);
            return true;
        }
    }

    return false;
}