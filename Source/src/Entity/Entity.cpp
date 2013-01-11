#include "Entity/Entity.hpp"

using namespace ic;

CEntity::~CEntity() {}

bool CEntity::LoadFromFile(
    const char* pmesh_filename,
    gfx::CVertexBuffer& VBO)
{
    if(m_Mesh.LoadMesh(pmesh_filename))
    {
        m_Mesh.LoadIntoVBO(VBO);
        return true;
    }

    return false;
}

void CEntity::Move(const float x, const float y)
{
    // Meh, really point-less premature optimization that ensures that
    // a new math::vector2_t won't be created every time Move() is called;
    // it's created on the stack so it's honestly like one CPU instruction
    // but whatever man don't judge me.
    static math::vector2_t Pos;
    Pos.x = x; Pos.y = y;

    m_Mesh.Move(Pos);
}

void CEntity::Adjust( const math::vector2_t& Rate )
{
    m_Mesh.Move(m_Mesh.GetPosition() + Rate);
}


