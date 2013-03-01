#include "GUI/Button.hpp"

using namespace ic;

using asset::CAssetManager;
using asset::CTexture;
using asset::CMesh;

using gui::CButton;
using gui::CFont;

CButton::CButton() : m_align(IC_ALIGN_CENTER)
{
    for(short i = 0; i < 3; ++i)
        mp_Textures[i] = CAssetManager::Create<CTexture>();
}

CButton::~CButton()
{
    for(short i = 0; i < 3; ++i)
        CAssetManager::Destroy<CTexture>(mp_Textures[i]);
}

bool CButton::Create(const char* texture,   const math::rect_t& Dim,
                     const char* text,      gfx::CScene& Scene,
                     const char* on_hover,   const char* on_down)
{
    gfx::CVertexBuffer VBO;
    gfx::CFrameBuffer& FBO = m_FBO;

    CTexture* pMainTexture  = CAssetManager::Create<CTexture>(texture);
    CTexture* pHoverTexture = (on_hover == NULL) ? NULL : 
                              CAssetManager::Create<CTexture>(on_hover);
    CTexture* pClickTexture = (on_down == NULL)  ? NULL : 
                              CAssetManager::Create<CTexture>(on_down);
    CMesh*    pMesh         = CAssetManager::Create<CMesh>();

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

    if(pMainTexture == NULL || pMesh == NULL || mp_Textures[0] == NULL || 
      !pMesh->LoadFromRaw(quad_v, 4, gfx::Globals::g_FullscreenIndices, 6))
    {
        CAssetManager::Destroy<CTexture>(pMainTexture);
        CAssetManager::Destroy<CMesh>(pMesh);
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

    // Adjust shader matrices to fit FBO.
    gfx::Globals::g_DefaultEffect.Enable();
    gfx::Globals::g_DefaultEffect.SetMatrix("mv", math::IDENTITY);
    gfx::Globals::g_DefaultEffect.SetMatrix("proj", 
        math::matrix4x4_t::Projection2D(Dim.w, Dim.h, 10, -10));

    // Draw the button texture and restore screen matrix.
    pMainTexture->Bind();
    VBO.Draw();
    pMainTexture->Unbind();

    // Cache text for speedier rendering.
    m_Font.SetProjection(Dim.w, Dim.h, 10, -10);
    m_Font.CacheText(text, m_Pos);
    
    // Render the font onto the button texture.
    m_Font.RenderCached();

    // Get texture from FBO.
    mp_Textures[0]->LoadFromTexture(FBO.GetTexture(), true);
    mp_Textures[0]->SetFilename(std::string(texture) + ":GUI");

    // Repeat for the hover-texture.
    if(pHoverTexture != NULL)
    {
        gfx::Globals::g_DefaultEffect.Enable();
        pHoverTexture->Bind();
        VBO.Draw();
        pMainTexture->Unbind();

        // Render the font onto the button texture.
        m_Font.RenderCached();

        // Get texture from FBO.
        mp_Textures[1]->LoadFromTexture(FBO.GetTexture(), true);
        mp_Textures[1]->SetFilename(std::string(texture) + ":GUI_Hover");
    }

    // Repeat for the click-texture.
    if(pClickTexture != NULL)
    {        
        gfx::Globals::g_DefaultEffect.Enable();
        pClickTexture->Bind();
        VBO.Draw();
        pClickTexture->Unbind();

        // Render the font onto the button texture.
        m_Font.RenderCached();
        m_Font.SetProjection(gfx::CWindow::GetProjectionMatrixC());

        // Get texture from FBO.
        mp_Textures[2]->LoadFromTexture(FBO.GetTexture(), true);
        mp_Textures[2]->SetFilename(std::string(texture) + ":GUI_Click");
    }

    gfx::Globals::g_DefaultEffect.Enable();
    gfx::Globals::g_DefaultEffect.SetMatrix("proj", 
        gfx::CWindow::GetProjectionMatrix());

    VBO.Clear();
    FBO.Disable();
    gfx::Globals::g_DefaultEffect.Disable();

    m_Entity.SetMaterialOverride(mp_Textures[0]);
    m_Entity.Move(Dim.x, Dim.y);
    
    m_text = text;
    return true;
}

bool CButton::LoadFont(CFont& pFont)
{
    m_Font = pFont;
    return true;
}
