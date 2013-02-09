#include "Entity/RigidBody.hpp"

using namespace ic;

bool CRigidBody::LoadFromFile(const char* pmesh_filename,
                              gfx::CVertexBuffer& VBO)
{
    bool val = CEntity::LoadFromFile(pmesh_filename, VBO);

    m_CollisionBox.w = CEntity::GetW();
    m_CollisionBox.h = CEntity::GetH();

    return val;
}

bool CRigidBody::LoadFromFile(const std::string& mesh_filename,
                              gfx::CVertexBuffer& VBO )
{
    bool val = CEntity::LoadFromFile(mesh_filename, VBO);

    m_CollisionBox.w = CEntity::GetW();
    m_CollisionBox.h = CEntity::GetH();

    return val;
}

bool CRigidBody::LoadFromMesh(asset::CMesh* pMesh,
                              gfx::CVertexBuffer& VBO)
{
    bool val = CEntity::LoadFromMesh(pMesh, VBO);

    m_CollisionBox.w = CEntity::GetW();
    m_CollisionBox.h = CEntity::GetH();

    return val;
}

void CRigidBody::Move(const math::vector2_t& Pos)
{
    CEntity::Move(Pos);

    m_CollisionBox.x = Pos.x;
    m_CollisionBox.y = Pos.y;
}

void CRigidBody::Move(const float x, const float y)
{
    CEntity::Move(x, y);

    m_CollisionBox.x = x;
    m_CollisionBox.y = y;
}

void CRigidBody::Adjust(const math::vector2_t& Rate)
{
    CEntity::Adjust(Rate);

    m_CollisionBox.x += Rate.x;
    m_CollisionBox.y += Rate.y;
}

void CRigidBody::Adjust(const float dx, const float dy)
{
    CEntity::Adjust(dx, dy);

    m_CollisionBox.x += dx;
    m_CollisionBox.y += dy;
}

void CRigidBody::AddVForce(const float dy)
{
    m_vert += dy;
}

void CRigidBody::AddHForce(const float dx)
{
    m_horz += dx;
}

void CRigidBody::Equalize()
{
    m_horz = m_vert = 0.f;
}

bool CRigidBody::CheckCollision(const CEntity* Other) const
{
    math::rect_t OtherBox;
    OtherBox.x = Other->GetX();
    OtherBox.y = Other->GetY();
    OtherBox.w = Other->GetW();
    OtherBox.h = Other->GetH();

    return m_CollisionBox.Collides(OtherBox);
}

bool CRigidBody::CheckCollision(const math::rect_t& Other) const
{
    return m_CollisionBox.Collides(Other);
}

bool CRigidBody::CheckCollision(const math::vector2_t& Other) const
{
    return this->CheckCollision(math::rect_t(Other.x, Other.y, 1, 1));
}

void CRigidBody::Update()
{
    this->Adjust(m_horz, m_vert);
}

math::vector2_t CRigidBody::GetForces() const
{
    return math::vector2_t(m_horz, m_vert);
}
