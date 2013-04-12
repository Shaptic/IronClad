#include "IronClad/Graphics/Globals.hpp"

using namespace ic;
using namespace ic::gfx;

CEffect         Globals::g_DefaultEffect;
CVertexBuffer   Globals::g_FullscreenVBO;

asset::CTexture* Globals::g_WhiteTexture;

vertex2_t       Globals::g_FullscreenVertices[4];
uint16_t        Globals::g_FullscreenIndices [6] = {0, 1, 3, 3, 2, 1};

bool Globals::Init(CWindow& Window)
{
    if(!(g_FullscreenVBO.Init() &&
         g_DefaultEffect.Init(IC_NO_EFFECT)))
    {
        return false;
    }

    g_DefaultEffect.Enable();
    g_DefaultEffect.SetMatrix("proj", Window.GetProjectionMatrix());
    g_DefaultEffect.SetMatrix("mv", math::IDENTITY);
    g_DefaultEffect.Disable();

    const unsigned char white[] = {'\xff', '\xff', '\xff', '\xff'};
    g_WhiteTexture = asset::CAssetManager::Create<asset::CTexture>();
    g_WhiteTexture->SetFilename("Global white texture");
    g_WhiteTexture->LoadFromRaw(GL_RGBA, GL_RGBA, 1, 1, white);

    g_FullscreenVBO.SetType(GL_STATIC_DRAW);
    LoadVBODefaults();
    return true;
}


bool ic::gfx::Globals::Init(math::matrix4x4_t& ProjectionMatrix)
{
    if(!(g_FullscreenVBO.Init() &&
         g_DefaultEffect.Init(IC_NO_EFFECT)))
    {
        return false;
    }

    g_DefaultEffect.Enable();
    g_DefaultEffect.SetMatrix("proj", ProjectionMatrix.GetMatrixPointer());
    g_DefaultEffect.SetMatrix("mv", math::IDENTITY);
    g_DefaultEffect.Disable();

    const unsigned char white[] = {'\xff', '\xff', '\xff', '\xff'};
    g_WhiteTexture = asset::CAssetManager::Create<asset::CTexture>();
    g_WhiteTexture->SetFilename("Global white texture");
    g_WhiteTexture->LoadFromRaw(GL_RGBA, GL_RGBA, 1, 1, white);

    g_FullscreenVBO.SetType(GL_STATIC_DRAW);
    LoadVBODefaults();
    return true;
}


void Globals::LoadVBODefaults()
{
    g_FullscreenVertices[0].Position = math::vector2_t(0,   0);
    g_FullscreenVertices[1].Position = math::vector2_t(800, 0);
    g_FullscreenVertices[2].Position = math::vector2_t(800, 600);
    g_FullscreenVertices[3].Position = math::vector2_t(0,   600);

    g_FullscreenVertices[0].TexCoord = math::vector2_t(0, 1);
    g_FullscreenVertices[1].TexCoord = math::vector2_t(1, 1);
    g_FullscreenVertices[2].TexCoord = math::vector2_t(1, 0);
    g_FullscreenVertices[3].TexCoord = math::vector2_t(0, 0);

    g_FullscreenVertices[0].Color = 
    g_FullscreenVertices[1].Color = 
    g_FullscreenVertices[2].Color = 
    g_FullscreenVertices[3].Color = color4f_t();

    g_FullscreenVBO.Clear();
    g_FullscreenVBO.AddData(g_FullscreenVertices, 4,
                            g_FullscreenIndices,  6);
    g_FullscreenVBO.FinalizeBuffer();
}

void Globals::LoadVBOCustom(const color4f_t& Color)
{
    g_FullscreenVertices[0].Position = math::vector2_t(0,   0);
    g_FullscreenVertices[1].Position = math::vector2_t(800, 0);
    g_FullscreenVertices[2].Position = math::vector2_t(800, 600);
    g_FullscreenVertices[3].Position = math::vector2_t(0,   600);

    g_FullscreenVertices[0].TexCoord = math::vector2_t(0, 1);
    g_FullscreenVertices[1].TexCoord = math::vector2_t(1, 1);
    g_FullscreenVertices[2].TexCoord = math::vector2_t(1, 0);
    g_FullscreenVertices[3].TexCoord = math::vector2_t(0, 0);

    g_FullscreenVertices[0].Color = 
    g_FullscreenVertices[1].Color = 
    g_FullscreenVertices[2].Color = 
    g_FullscreenVertices[3].Color = Color;

    g_FullscreenVBO.Clear();
    g_FullscreenVBO.AddData(g_FullscreenVertices, 4,
                            g_FullscreenIndices,  6);
    g_FullscreenVBO.FinalizeBuffer();
}
