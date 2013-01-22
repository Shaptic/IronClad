#include "Graphics/Scene.hpp"

using namespace ic;
using gfx::CScene;
using util::g_Log;
using gfx::Globals;

gfx::material_t CScene::m_ShadowShader;

CScene::CScene(gfx::CWindow& Window, const gfx::SceneType scene) : 
    m_Window(Window), m_postfx(true), m_lighting(true),
    m_geo_type(GL_TRIANGLES)
{
    switch(scene)
    {
    case IC_ANY_SCENE:
    case IC_DYNAMIC_SCENE:
        m_GeometryVBO.SetType(GL_DYNAMIC_DRAW);
        break;

    case IC_STATIC_SCENE:
        m_GeometryVBO.SetType(GL_STATIC_DRAW);
        break;
    }

    m_ShadowVBO.SetType(GL_DYNAMIC_DRAW);
}

CScene::~CScene(){}

bool CScene::Init()
{
    // Initialize frame-buffer.
    return (m_GeometryVBO.Init()                            &&
            m_ShadowVBO.Init()                              &&
            m_FBO.Init(m_Window.GetW(), m_Window.GetH())    &&
            m_FBOSwap.Init(m_Window.GetW(), m_Window.GetH()));
}

CEntity* CScene::AddMesh(
    const std::string& filename,
    const math::vector2_t& Pos,
    bool anim,
    bool rigid)
{
    CEntity* pFinal = NULL;

    if (anim)       pFinal = new CAnimation;
    else if (rigid) pFinal = new CRigidBody;
    else            pFinal = new CEntity;

    if(!pFinal->LoadFromFile(filename, m_GeometryVBO))
    {
        delete pFinal;
        return (pFinal = NULL);
    }

    pFinal->Move(Pos);
    mp_sceneObjects.push_back(pFinal);
    return pFinal;
}

CEntity* CScene::AddMesh(asset::CMesh* pMesh, const math::vector2_t& Pos)
{
    CEntity* pFinal = new CEntity;
    if(!pFinal->LoadFromMesh(pMesh, m_GeometryVBO))
    {
        delete pFinal;
        return (pFinal = NULL);
    }

    pFinal->Move(Pos);
    mp_sceneObjects.push_back(pFinal);
    return pFinal;
}

/**
 * @todo    Shadow generation.
 **/
void CScene::Render()
{
    // Model-view matrix.
    static math::matrix4x4_t MVMatrix = math::IDENTITY;

    m_ShadowVBO.Clear();
    m_GeometryVBO.FinalizeBuffer();

    // Clear swap frame-buffer.
    m_FBOSwap.Enable();
    m_FBOSwap.Clear();
    m_FBOSwap.Disable();

    // Enable render-to-texture.
    m_FBO.Enable();
    m_FBO.Clear();

    // Clear frame-buffer settings.
    glClearColor(0.1f, 0.1f, 0.1f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    

    // Blending is essential.
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

    // Normal transparency blending function.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Bind geometry VBO.
    m_GeometryVBO.Bind();

    // Render all of the meshes.
    for(size_t i = 0; i < mp_sceneObjects.size(); ++i)
    {
        // Load the model-view matrix.
        mp_sceneObjects[i]->GetMesh().LoadPositionMatrix(MVMatrix);

        // Adjust for the camera.
        MVMatrix[0][3] += m_Camera.x;
        MVMatrix[1][3] += m_Camera.y;
        
        // Get mesh surfaces.
        const std::vector<gfx::surface_t*>& meshSurfaces = 
            mp_sceneObjects[i]->GetMesh().GetSurfaces();

        // Quads get one single texture, this accounts for animation.
        if(meshSurfaces.size() == 1)
        {
            this->StandardRender(mp_sceneObjects[i], MVMatrix);
        }
        else
        {
            if(!mp_sceneObjects[i]->IsRenderable()) continue;

            // Render each surface.
            for(size_t j = 0; j < meshSurfaces.size(); ++j)
            {
                this->StandardRender(meshSurfaces[j], MVMatrix);
            }
        }
    }

    uint32_t final_texture = m_FBO.GetTexture();

    if(m_lighting)
    {
        Globals::g_FullscreenVBO.Bind();

        // Render effects on the scene.
        m_FBOSwap.Enable();
        glBindTexture(GL_TEXTURE_2D, m_FBO.GetTexture());        

        // Additive blending for lighting.
        glBlendFunc(GL_ONE, GL_ONE);

        // Render all lights onto the frame-buffer.
        for(size_t i = 0; i < mp_sceneLights.size(); ++i)
        {
            this->LightRender(mp_sceneLights[i]);
        }

        // Final FBO texture to render onto the screen.
        // This texture has all of the post-processing on it.
        final_texture = m_FBOSwap.GetTexture();
    }

    if(m_postfx)
    {
        // Render each effect onto the scene.
        // The FBOs are swapped every time,
        // adding effects on top of one another.
        for(size_t i = 0; i < mp_sceneEffects.size(); ++i)
        {
            final_texture = this->PostProcessingRender(
                mp_sceneEffects[i],
                i % 2 == 0 ? m_FBOSwap : m_FBO,
                i % 2 == 0 ? m_FBO.GetTexture() : m_FBOSwap.GetTexture());
        }
    }

    // This is so the FBO blends with the data in the default frame-buffer.
    glDisable(GL_BLEND);
    glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ZERO, GL_ONE);

    // Render to the screen now.
    m_FBOSwap.Disable();
    Globals::g_FullscreenVBO.Bind();
    
    // Draw off-screen texture to screen.
    Globals::g_DefaultEffect.Enable();
    Globals::g_DefaultEffect.SetMatrix("mv", math::IDENTITY.GetMatrixPointer());
    
    glBindTexture(GL_TEXTURE_2D, final_texture);
    Globals::g_FullscreenVBO.Draw();
    glBindTexture(GL_TEXTURE_2D, 0);

    Globals::g_DefaultEffect.Disable();
    Globals::g_FullscreenVBO.Unbind();
}

bool CScene::AddMaterialOverlay(gfx::CEffect* pEffect)
{
    if(pEffect == NULL) return false;

    // Set up matrices, just in case.
    pEffect->Enable();
    pEffect->SetMatrix("mv", ic::math::IDENTITY.GetMatrixPointer());
    pEffect->SetMatrix("proj", m_Window.GetProjectionMatrix());
    pEffect->Disable();

    mp_sceneEffects.push_back(pEffect);
    return true;
}

void CScene::StandardRender(CEntity* pEntity,
                            const math::matrix4x4_t& ModelView)
{
    if(!pEntity->IsRenderable()) return;

    gfx::surface_t* pSurface    = pEntity->GetMesh().GetSurfaces()[0];
    gfx::material_t* pMaterial  = pSurface->pMaterial;

    // Nothing to render if there's no texture/shader.
    if(pMaterial == NULL) return;

    // Bind texture and shader, returns false if no shader,
    // so bind the default.
    if(!pMaterial->pShader)
    {
        Globals::g_DefaultEffect.Enable();
        Globals::g_DefaultEffect.SetMatrix("mv", ModelView);
    }
    else
    {
        pMaterial->pShader->Bind();
        int mvloc = pMaterial->pShader->GetUniformLocation("mv");
        int pjloc = pMaterial->pShader->GetUniformLocation("proj");

        if(mvloc == -1 || pjloc == -1) return;

        glUniformMatrix4fv(mvloc, 1, GL_TRUE,
            ModelView.GetMatrixPointer());

        glUniformMatrix4fv(pjloc, 1, GL_TRUE,
            m_Window.GetProjectionMatrix());
    }

    if(pEntity->GetTexture() != NULL)
        pEntity->GetTexture()->Bind();

    // Do rendering.
    glDrawElements(m_geo_type, pSurface->icount, GL_UNSIGNED_SHORT,
        (void*)(sizeof(uint16_t)*(pSurface->start)));

    // Unbind shader / texture.
    glBindTexture(GL_TEXTURE_2D, 0);
    Globals::g_DefaultEffect.Disable();
}

void CScene::StandardRender(gfx::surface_t* pSurface,
                            const math::matrix4x4_t& ModelView)
{
    gfx::material_t* pMaterial = pSurface->pMaterial;

    // Nothing to render if there's no texture/shader.
    if(pMaterial == NULL) return;

    // Bind texture and shader, returns false if no shader,
    // so bind the default.
    if(!pMaterial->Bind())
    {
        Globals::g_DefaultEffect.Enable();
        Globals::g_DefaultEffect.SetMatrix("mv", ModelView);
    }
    else
    {
        int mvloc = pMaterial->pShader->GetUniformLocation("mv");
        int pjloc = pMaterial->pShader->GetUniformLocation("proj");

        if(mvloc == -1 || pjloc == -1) return;

        glUniformMatrix4fv(mvloc, 1, GL_TRUE,
            ModelView.GetMatrixPointer());

        glUniformMatrix4fv(pjloc, 1, GL_TRUE,
            m_Window.GetProjectionMatrix());
    }

    // Do rendering.
    glDrawElements(
        m_geo_type,                                     // Tris, lines, ...
        pSurface->icount,                               // Index count
        GL_UNSIGNED_SHORT,                              // uint16_t indices
        (void*)(sizeof(uint16_t)*(pSurface->start)));   // Buffer offset

    // Unbind shader / texture.
    if(!pMaterial->Unbind()) Globals::g_DefaultEffect.Disable();
}

void CScene::LightRender(gfx::CLight* pLight)
{
    // Nothing to render if there's no texture/shader.
    if(pLight == NULL) return;

    // Bind post-effect.
    pLight->Enable();
    pLight->SetPosition(pLight->GetPosition() + m_Camera);

    // Draw off-screen texture to screen.
    Globals::g_FullscreenVBO.Draw();

    // Turn off effect.
    pLight->SetPosition(pLight->GetPosition() - m_Camera);
    pLight->Disable();
}

uint32_t CScene::PostProcessingRender(gfx::CEffect* pEffect,
    gfx::CFrameBuffer& Target, uint32_t source)
{
    if(pEffect == NULL) return 0;

    pEffect->Enable();
    Target.Enable();
    glBindTexture(GL_TEXTURE_2D, source);
    Globals::g_FullscreenVBO.Draw();
    glBindTexture(GL_TEXTURE_2D, 0);
    Target.Disable();
    pEffect->Disable();

    return Target.GetTexture();
}

/**
 * @todo    Make it actually find the ID of an existing light.
 **/
uint16_t CScene::AddLight(gfx::CLight* pLight)
{
    if(pLight == NULL) return -1;

    std::vector<gfx::CLight*>::iterator finder = std::find(
        mp_sceneLights.begin(), mp_sceneLights.end(), pLight);

    if(finder == mp_sceneLights.end())
    {
        mp_sceneLights.push_back(pLight);
        return mp_sceneLights.size() - 1;
    }

    return -1;
}

void CScene::ShadowRender()
{
    m_ShadowVBO.Draw();
}

void CScene::UpdateShadows(const math::vector2_t& LightPosition)
{
    // Retrieve buffers from the GPU.
    vertex2_t* vertices = (vertex2_t*)m_GeometryVBO.GetTemporaryBuffer(
        GL_ARRAY_BUFFER);

    uint16_t* indices = (uint16_t*)m_GeometryVBO.GetTemporaryBuffer(
        GL_ELEMENT_ARRAY_BUFFER);

    if(!vertices || !indices)
    {
        g_Log.Flush();
        g_Log << "[ERROR] Failed to map vertex buffer.\n";
        g_Log << "[ERROR] Most likely caused by an improper bind.\n";
        g_Log << "[ERROR] OpenGL error: " << glGetString(glGetError());
        g_Log << "\n";
        g_Log.PrintLastLog();

        handle_error(g_Log.GetLastLog());
    }

    // Determine buffer sizes.
    uint32_t vsize = m_GeometryVBO.GetVCount();
    uint32_t isize = m_GeometryVBO.GetICount();

    if(!vsize || !isize)
    {
        g_Log.Flush();
        g_Log << "[ERROR] Empty buffers.\n";
        g_Log << "[ERROR] OpenGL error code: " << glGetError() << "\n";
        g_Log.PrintLastLog();
        return;
    }

    for(size_t j = 0; j < mp_sceneObjects.size(); ++j)
    {
        if(!mp_sceneObjects[j]->CastsShadow()) continue;

        // Find the lowest and highest indices to get the clock-wise ordered
        // vertices from the buffer.
        std::vector<gfx::surface_t*>& Surfaces = 
            mp_sceneObjects[j]->GetMesh().GetSurfaces();

        uint16_t mindex = indices[Surfaces.front()->start];
        uint16_t maxdex = indices[Surfaces.front()->start];

        for(size_t k = Surfaces.front()->start;
            k < Surfaces.back()->start + Surfaces.back()->icount;
            ++k)
        {
            if(indices[k] < mindex) mindex = indices[k];
            if(indices[k] > maxdex) maxdex = indices[k];
        }

        // We need to track which vertex is closest to the light source
        // in order to make the shadows render in a clock-wise order.
        uint16_t closest_vertex = 0;

        math::vector2_t Position = vertices[mindex].Position + m_Camera + 
            mp_sceneObjects[j]->GetMesh().GetPosition();

        float closest_distance  = math::distance(
            Position.x, Position.y,
            LightPosition.x, LightPosition.y, false);

        // Ray-trace valid shadow outlines.
        for(size_t i = mindex; i <= maxdex; ++i)
        {
            math::vector2_t Position = vertices[i].Position + m_Camera + 
                mp_sceneObjects[j]->GetMesh().GetPosition();

            // Line from the vertex to the next one.
            math::vector2_t CasterLine = 
                vertices[(i == maxdex) ? 0 : i + 1].Position - 
                vertices[i].Position;

            // Vector from the light to the vertex.
            math::vector2_t LightToVertex = Position - LightPosition;

            // Vector normal to the caster line, facing the right.
            math::vector2_t Normal(CasterLine.y, -CasterLine.x);

            // A line will cast a shadow if N . L >= 0
            //if(Normal * LightToVertex >= 0.f)
            {
                vertex2_t Final1, Final2;

                Final1.Position = Position;
                Final2.Position = (Position - LightPosition) * 4096.f;

                m_shadowVertices.reserve(m_shadowVertices.size() + 2);
                m_shadowVertices.push_back(Final1);
                m_shadowVertices.push_back(Final2);

                // Check distance from light.
                float this_distance = 
                    LightToVertex.x * LightToVertex.x + 
                    LightToVertex.y * LightToVertex.y;

                if(closest_vertex == -1)
                {
                    closest_vertex = m_shadowVertices.size() - 2;
                    closest_distance = this_distance;
                }
                else if(this_distance < closest_distance)
                {
                    closest_distance = this_distance;
                    closest_vertex = m_shadowVertices.size() - 2;
                }
            }
        }

        // Add indices for the shadow vertices to actually make the shadow.
        m_shadowIndices.reserve(m_shadowIndices.size() + 
            (3 * m_shadowVertices.size() + 6) / 2);

        // Add indices for the shadows in a clock-wise fashion.
        uint8_t counter = 0;
        size_t j = closest_vertex;

        for( ; j < m_shadowVertices.size() - 2; ++j)
        {
            if(j % 2 == 0)
            {
                m_shadowIndices.push_back(j);
                m_shadowIndices.push_back(j + 1);
                m_shadowIndices.push_back(j + 2);
            }
            else
            {
                m_shadowIndices.push_back(j + 1);
                m_shadowIndices.push_back(j + 2);
                m_shadowIndices.push_back(j + 3);
            }
        }

        m_shadowIndices.push_back(j);
        m_shadowIndices.push_back(j + 1);
        m_shadowIndices.push_back(0);
        m_shadowIndices.push_back(j + 1);
        m_shadowIndices.push_back(0);
        m_shadowIndices.push_back(1);

        for(size_t i = 0; i + 2 < closest_vertex; ++i)
        {
            if(i % 2 == 0)
            {
                m_shadowIndices.push_back(i);
                m_shadowIndices.push_back(i + 1);
                m_shadowIndices.push_back(i + 2);
            }
            else
            {
                m_shadowIndices.push_back(i + 1);
                m_shadowIndices.push_back(i);
                m_shadowIndices.push_back(i + 2);
            }
        }
    }

    // Add shadow data to the VBO.
    m_ShadowVBO.AddData(&m_shadowVertices[0], m_shadowVertices.size(),
        &m_shadowIndices[0], m_shadowIndices.size());

    m_shadowIndices.clear();
    m_shadowVertices.clear();

    // Done with buffers.
    /// @todo Make this a part of the VBO wrapper.
    glUnmapBuffer(GL_ARRAY_BUFFER);
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
}

bool CScene::RemoveLight(const uint16_t id)
{
    if(id > mp_sceneLights.size())
        return false;

    mp_sceneLights.erase(mp_sceneLights.begin() + id);
    return true;
}

bool CScene::RemoveLight(const gfx::CLight* pLight)
{
    for(size_t i = 0; i < mp_sceneLights.size(); ++i)
    {
        if(mp_sceneLights[i] == pLight)
        {
            mp_sceneLights.erase(mp_sceneLights.begin() + i);
            return true;
        }
    }

    return false;
}

bool gfx::CScene::InsertMesh(const uint16_t position, CEntity* pEntity)
{
    if(mp_sceneObjects.size() < position) return false;

    mp_sceneObjects.insert(mp_sceneObjects.begin() + position, pEntity);
    return true;
}

int CScene::GetQueuePosition(const CEntity* pEntity) const
{
    for(size_t i = 0; i < mp_sceneObjects.size(); ++i)
    {
        if(mp_sceneObjects[i] == pEntity) return i;
    }

    return -1;
}
