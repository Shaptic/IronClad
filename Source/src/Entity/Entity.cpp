#include "IronClad/Entity/Entity.hpp"

using namespace ic;
using obj::CEntity;

CEntity::~CEntity()
{
    asset::CAssetManager::Destroy(mp_Override);
}

bool CEntity::LoadFromFile(const char* pmesh_filename,
                           gfx::CVertexBuffer& VBO)
{
    m_Mesh.SetSceneOwner(&VBO);
    return m_Mesh.LoadMesh(pmesh_filename) && m_Mesh.LoadIntoVBO(VBO);
}

bool CEntity::LoadFromFile(const std::string& mesh_filename,
                           gfx::CVertexBuffer& VBO)
{
    return this->LoadFromFile(mesh_filename.c_str(), VBO);
}

bool CEntity::LoadFromMesh(asset::CMesh* pMesh, gfx::CVertexBuffer& VBO)
{
    m_Mesh.mp_ActiveMesh = pMesh;
    m_Mesh.SetSceneOwner(&VBO);
    pMesh->SetOwner(&VBO);
    return m_Mesh.LoadIntoVBO(VBO);
}

void CEntity::Move(const float x, const float y)
{
    // [Update] I'm keeping this here because I LOL @ my own stupidity:
    // 
    // Meh, really pointless premature optimization that ensures that
    // a new math::vector2_t won't be created every time Move() is called;
    // it's created on the stack so it's honestly like one CPU instruction
    // but whatever man don't judge me.
    static math::vector2_t Pos;
    Pos.x = x; Pos.y = y;

    m_Mesh.Move(Pos);
}

void CEntity::Move(const math::vector2_t& Pos)
{
    m_Mesh.Move(Pos);
}

void CEntity::Adjust(const math::vector2_t& Rate)
{
    m_Mesh.Move(m_Mesh.GetPosition() + Rate);
}

void CEntity::Adjust(const float dx, const float dy)
{
    this->Move(this->GetX() + dx, this->GetY() + dy);
}

void CEntity::SetMaterialOverride(asset::CTexture* pTexture)
{
    mp_Override = pTexture;
}

bool CEntity::LoadFromImage(const char* pimg_name,
                            gfx::CVertexBuffer& VBO)
{
    vertex2_t v[4];
    uint16_t  i[6] = {0, 1, 3, 3, 1, 2};

    asset::CTexture* pTGA = asset::CAssetManager::Create
                                          <asset::CTexture>(pimg_name);

    if(pTGA == NULL) return false;

    v[0].Position = math::vector2_t(0.f,            0.f);
    v[1].Position = math::vector2_t(pTGA->GetW(),   0.f);
    v[2].Position = math::vector2_t(pTGA->GetW(),   pTGA->GetH());
    v[3].Position = math::vector2_t(0.f,            pTGA->GetH());

    v[0].TexCoord = math::vector2_t(0, 1);
    v[1].TexCoord = math::vector2_t(1, 1);
    v[2].TexCoord = math::vector2_t(1, 0);
    v[3].TexCoord = math::vector2_t(0, 0);
    
    asset::CMesh* pMesh = asset::CAssetManager::Create<asset::CMesh>(&VBO);

    this->SetMaterialOverride(pTGA);
    return pMesh->LoadFromRaw(v, 4, i, 6) && this->LoadFromMesh(pMesh, VBO);
}
