#include "Entity/Animation.hpp"

using namespace ic;
using util::g_Log;

bool CAnimation::LoadFromFile(const std::string& filename,
                              gfx::CVertexBuffer& VBO)
{
    std::ifstream anim(filename, std::ios::binary);

    CAnimation::AnimationHeader& header = m_SheetDetails;
    memset(&header, NULL, sizeof header);

    if(!anim.is_open()) return false;

    // Read header.
    anim >> header.width;
    anim >> header.height;
    anim >> header.columns;
    anim >> header.rows;

    // Bad header if it has any zeroes.
    if(!(header.width && header.height && header.columns && header.rows))
        return false;

    // Get file size.
    const std::streampos begin = anim.tellg();
    anim.seekg(0, std::ios::end);
    const std::streampos end = anim.tellg();
    anim.seekg(begin);

    // Texture data.
    unsigned char* data = new unsigned char[end - begin];
    anim.read((char*)data, end - begin);

    GLFWimage img;
    glfwReadMemoryImage(data, end - begin, &img, GLFW_NO_RESCALE_BIT);

    uint16_t w = (header.width);
    uint16_t h = (header.height);

    uint16_t sprite_w = w / header.columns;
    uint16_t sprite_h = h / header.rows;

    vertex2_t quad_v[4];
    uint16_t  quad_i[6] = {0, 1, 3, 3, 2, 1};

    m_TexcDim = math::vector2_t(1.f / header.columns, 1.f / header.rows);

    quad_v[0].Position = math::vector2_t(0,         0);
    quad_v[1].Position = math::vector2_t(sprite_w,  0);
    quad_v[2].Position = math::vector2_t(sprite_w,  sprite_h);
    quad_v[3].Position = math::vector2_t(0,         sprite_h);

    quad_v[0].TexCoord = math::vector2_t(0.f,   1.f);
    quad_v[1].TexCoord = math::vector2_t(1.f,   1.f);
    quad_v[2].TexCoord = math::vector2_t(1.f,   0.f);
    quad_v[3].TexCoord = math::vector2_t(0.f,   0.f);

    g_Log.Flush();
    g_Log << "[DEBUG] Loading animation file. ";
    g_Log << (w / sprite_w) * (h / sprite_h) << " sprite(s) expected.\n";
    g_Log.PrintLastLog();

    std::stringstream ss;

    asset::CTexture* pTexture = asset::CAssetManager::Create<asset::CTexture>();
    pTexture->SetFilename(filename);
    pTexture->LoadFromRaw(img.Format, img.Format, w, h, img.Data);
    header.pTexture = pTexture;

    glfwFreeImage(&img);

    // Load mesh
    m_Mesh.LoadMesh(quad_v, 4, quad_i, 6);

    // Load whole atlas into surface, then load custom shader.
    m_Mesh.GetSurfaces()[0]->pMaterial->pTexture = pTexture;

    gfx::CShaderPair* pShader = m_Mesh.GetSurfaces()[0]->pMaterial->pShader;
    if(pShader == NULL)
    {
        pShader = new gfx::CShaderPair;
        if(!pShader->LoadFromFile("Shaders/Default.vs", "Shaders/Animate.fs"))
            return false;
    }
    
    // The shader contains an offset so that the texture coordinates
    // can be shifted to reflect the current sprite.
    pShader->Bind();
    m_tc_loc = pShader->GetUniformLocation("tc_offset");
    m_tc_str = pShader->GetUniformLocation("tc_start");
    glUniform2f(m_tc_loc, m_TexcDim.x, m_TexcDim.y);
    pShader->Unbind();
    m_Mesh.GetSurfaces()[0]->pMaterial->pShader = pShader;

    // Rigid body collision.
    m_CollisionBox.w = m_SheetDetails.width  / m_SheetDetails.columns;
    m_CollisionBox.h = m_SheetDetails.height / m_SheetDetails.rows;

    return m_Mesh.LoadIntoVBO(VBO);
}

bool CAnimation::NextSprite()
{
    if(++m_active >= this->GetAnimationCount())
    {
        ++m_loops_done;
        m_active = 0;
    }

    util::g_Log.Flush();
    util::g_Log << "[DEBUG] Rendering texture[" << (int)m_active << "]: ";
    util::g_Log << m_Mesh.GetSurfaces()[0]->pMaterial->pTexture->GetFilename() << "\n";
    util::g_Log.PrintLastLog();

    // Adjust for current texture
    gfx::CShaderPair* pShader = m_Mesh.GetSurfaces()[0]->pMaterial->pShader;
    pShader->Bind();
    glUniform2f(m_tc_str, m_active * m_TexcDim.x, m_active * m_TexcDim.y);
    pShader->Unbind();

    return (m_active > 0);
}

void CAnimation::Update()
{
    CRigidBody::Update();

    if(util::CTimer::GetTimeElapsed() - m_last >= m_delay && m_enabled)
    {
        this->NextSprite();
        m_last = util::CTimer::GetTimeElapsed();
    }
}

void CAnimation::SetAnimationRate(const float delta)
{
    m_delay = delta;
}

void CAnimation::SwapSpriteSheet(const AnimationHeader& Header)
{
    m_Mesh.GetSurfaces()[0]->pMaterial->pTexture = Header.pTexture;
    m_SheetDetails  = Header;
    m_TexcDim       = math::vector2_t(1.f / Header.columns, 1.f / Header.rows);
    m_loops_done    = 0;

    // Adjust for new texture dimensions.
    gfx::CShaderPair* pShader = m_Mesh.GetSurfaces()[0]->pMaterial->pShader;
    pShader->Bind();
    glUniform2f(m_tc_loc, m_TexcDim.x, m_TexcDim.y);
    pShader->Unbind();
}
