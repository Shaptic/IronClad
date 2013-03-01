#include "GUI/Button.hpp"

using namespace ic;
using gui::CButton;
using gui::CFont;

CButton::CButton() : m_align(IC_ALIGN_CENTER), mp_Texture(NULL)
{
    mp_Texture = asset::CAssetManager::Create<asset::CTexture>();
}

CButton::~CButton()
{
    asset::CAssetManager::Destroy<asset::CTexture>(mp_Texture);
}

bool CButton::Create(const char* texture, const math::rect_t& Dim,
                     const char* text, gfx::CScene& Scene)
{
    gfx::CVertexBuffer VBO;
    gfx::CFrameBuffer& FBO = m_FBO;

    asset::CTexture* pTexture = asset::CAssetManager::Create<asset::CTexture>(texture);            
    asset::CMesh*    pMesh    = asset::CAssetManager::Create<asset::CMesh>();

    vertex2_t quad_v[4];

    quad_v[0].Position = math::vector2_t(0, 0);
    quad_v[1].Position = math::vector2_t(Dim.w, 0.f);
    quad_v[2].Position = math::vector2_t(Dim.w, Dim.h);
    quad_v[3].Position = math::vector2_t(0.f,   Dim.h);

    // For w/o FBOs.
    //quad_v[0].Position = math::vector2_t(Dim.x,         Dim.y);
    //quad_v[1].Position = math::vector2_t(Dim.x + Dim.w, Dim.y);
    //quad_v[2].Position = math::vector2_t(Dim.x + Dim.w, Dim.y + Dim.h);
    //quad_v[3].Position = math::vector2_t(Dim.x,         Dim.y + Dim.h);

    quad_v[0].TexCoord = math::vector2_t(0, 1);
    quad_v[1].TexCoord = math::vector2_t(1, 1);
    quad_v[2].TexCoord = math::vector2_t(1, 0);
    quad_v[3].TexCoord = math::vector2_t(0, 0);

    // Calculate offsets.
    float x_off = 0.f, y_off = 0.f;
    if(m_align == IC_ALIGN_CENTER)
    {
        x_off = Dim.w / 2.f - m_Font.GetTextWidth(text) / 2.f;
    }

    // Overflows, oops.
    uint32_t th = m_Font.GetTextHeight(text);
    int32_t h   = Dim.h;

    y_off = (h - th) / 2.f;

    m_Pos = math::vector2_t(x_off, Dim.h - y_off);

    // For without FBOs.
    //m_Pos = math::vector2_t(Dim.x + x_off, Dim.y + Dim.h - y_off);

    if( pTexture == NULL || pMesh == NULL || mp_Texture == NULL || 
        !pMesh->LoadFromRaw(quad_v, 4, gfx::Globals::g_FullscreenIndices, 6))
    {
        asset::CAssetManager::Destroy<asset::CTexture>(pTexture);
        asset::CAssetManager::Destroy<asset::CMesh>(pMesh);
        return false;
    }

    VBO.Init();
    FBO.Init(Dim.w, Dim.h);
    FBO.Enable();

    // Load the mesh into a local VBO.
    m_Entity.LoadFromMesh(pMesh, VBO);
    m_Entity.GetMesh().ClearMesh();
    VBO.FinalizeBuffer();

    // Load it again into the scene geometry VBO.
    pMesh->LoadFromRaw(quad_v, 4, gfx::Globals::g_FullscreenIndices, 6);
    m_Entity.LoadFromMesh(pMesh, Scene.GetGeometryBuffer());

    // Cache text for speedier rendering.
    //m_Font.CacheText(text, m_Pos);

    // Adjust shader matrices to fit FBO.
    gfx::Globals::g_DefaultEffect.Enable();
    gfx::Globals::g_DefaultEffect.SetMatrix("mv", math::IDENTITY);
    gfx::Globals::g_DefaultEffect.SetMatrix("proj", 
        math::matrix4x4_t::Projection2D(Dim.w, Dim.h, 10, -10));

    // Draw the button texture and restore screen matrix.
    pTexture->Bind();
    VBO.Draw();
    gfx::Globals::g_DefaultEffect.SetMatrix("proj", 
        gfx::CWindow::GetProjectionMatrix());
    pTexture->Unbind();

    // Render the font onto the button texture.
    m_Font.SetProjection(Dim.w, Dim.h, 10, -10);
    m_Font.RenderText(text, m_Pos);
    m_Font.SetProjection(gfx::CWindow::GetProjectionMatrixC());

    VBO.Clear();
    FBO.Disable();

    // Get texture from FBO.
    mp_Texture->LoadFromTexture(FBO.GetTexture());
    mp_Texture->SetFilename(std::string(texture) + ":GUI");
    m_Entity.SetMaterialOverride(mp_Texture);
    m_Entity.Move(Dim.x, Dim.y);
    
    m_text = text;
    return true;
}

bool CButton::LoadFont(CFont& pFont)
{
    m_Font = pFont;
    return true;
}
