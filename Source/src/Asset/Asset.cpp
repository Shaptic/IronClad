#include "IronClad/Asset/Asset.hpp"

using namespace ic;
using ic::asset::CAsset;

uint32_t CAsset::hash_seed;

CAsset::CAsset(bool original /*= false*/) : m_original(original),
                                            m_last_error("No error"),
                                            m_id(Hash((char*)this,
                                                sizeof(CAsset))) 
{}

CAsset::~CAsset()
{
    //if(m_original) this->Release();
}

void CAsset::Release()
{
    util::g_Log.Flush();
    util::g_Log << "[INFO] Deleted asset:     (";
    util::g_Log.SetWidth(10);
    util::g_Log << m_id << ") " << m_filename << "\n";
    util::g_Log.PrintLastLog();
}
