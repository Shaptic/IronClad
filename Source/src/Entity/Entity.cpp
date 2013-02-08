#include "Entity/Entity.hpp"

using namespace ic;

CEntity::~CEntity() {}

bool CEntity::LoadFromFile(
    const char* pmesh_filename,
    gfx::CVertexBuffer& VBO)
{
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
