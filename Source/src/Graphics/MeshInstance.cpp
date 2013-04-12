#include "IronClad/Graphics/MeshInstance.hpp"

using namespace ic;
using gfx::CMeshInstance;

CMeshInstance::CMeshInstance() : mp_ActiveMesh(NULL),
    m_RotationZ(1.f, 0.f), m_RotationX(1.f, 0.f), m_RotationY(1.f, 0.f),
    m_hflip(false), m_vflip(false)
{
    memset(m_degrees, 0, sizeof(m_degrees));
}

CMeshInstance& CMeshInstance::operator=(const CMeshInstance& Copy)
{
    m_Position      = Copy.m_Position;
    m_Dimensions    = Copy.m_Dimensions;
    mp_ActiveMesh   = Copy.mp_ActiveMesh;

    return (*this);
}

bool CMeshInstance::LoadMesh(asset::CMesh* pMesh)
{
    if(pMesh) mp_ActiveMesh = pMesh;
    return (pMesh != NULL);
}

bool CMeshInstance::LoadMesh(const std::string& filename)
{
    mp_ActiveMesh = asset::CAssetManager::Create<asset::CMesh>(filename);
    return (mp_ActiveMesh != NULL);
}

bool CMeshInstance::LoadIntoVBO(gfx::CVertexBuffer& VBO)
{
    if(!mp_ActiveMesh) return false;

    m_Dimensions.x = mp_ActiveMesh->GetMeshWidth();
    m_Dimensions.y = mp_ActiveMesh->GetMeshHeight();

    return mp_ActiveMesh->Offload(VBO);
}

bool CMeshInstance::LoadMesh(const vertex2_t* verts, const uint16_t vsize,
                             const uint16_t*  inds,  const uint16_t isize)
{
    if(mp_ActiveMesh != NULL)
        asset::CAssetManager::Destroy<asset::CMesh>(mp_ActiveMesh);

    mp_ActiveMesh = asset::CAssetManager::Create<asset::CMesh>();
    mp_ActiveMesh->SetFilename("Raw mesh");
    return mp_ActiveMesh->LoadFromRaw(verts, vsize, inds, isize);
}
