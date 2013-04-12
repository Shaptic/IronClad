/**
 * @file
 *  Graphics/Scene.hpp - Declarations for the CScene class, 
 *  a high-level interface containing information about a scene.
 *
 * @author      George Kudrayvtsev (halcyon)
 * @version     1.4.2
 * @copyright   Apache License v2.0
 *  Licensed under the Apache License, Version 2.0 (the "License").         \n
 *  You may not use this file except in compliance with the License.        \n
 *  You may obtain a copy of the License at:
 *  http://www.apache.org/licenses/LICENSE-2.0                              \n
 *  Unless required by applicable law or agreed to in writing, software     \n
 *  distributed under the License is distributed on an "AS IS" BASIS,       \n
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\n
 *  See the License for the specific language governing permissions and     \n
 *  limitations under the License.
 *
 * @addtogroup Graphics
 * @{p
 **/

#ifndef IRON_CLAD__GRAPHICS__SCENE_HPP
#define IRON_CLAD__GRAPHICS__SCENE_HPP

#include <vector>

#include "IronClad/Math/Line2.hpp"

#include "IronClad/Entity/Entity.hpp"
#include "IronClad/Entity/Animation.hpp"
#include "IronClad/Entity/RigidBody.hpp"

#include "Globals.hpp"
#include "Framebuffer.hpp"
#include "MeshInstance.hpp"
#include "Material.hpp"
#include "Light.hpp"
#include "Effect.hpp"

namespace ic
{
    /**
     * Forward declaration.
     **/
    class CLevel;

namespace gfx
{
    /**
     * Hints for scene creation, specify how often new meshes
     * will be added to the scene after the first Render() call.
     **/
    enum SceneType
    {
        IC_ANY_SCENE,
        IC_DYNAMIC_SCENE,
        IC_STATIC_SCENE
    };

    /**
     * A collection of various graphical elements that interact together.
     *  A scene is the high-level rendering interface in IronClad, taking
     *  care of many details under the hood. Calls to AddMesh will 
     *  add objects in various positions, and you can modify those directly
     *  for movement and other functions. Similarly, you can add scene-wide
     *  shaders, effects, lights.
     *  
     * @bug There can only be a single scene created at a time, because
     *      meshes are loaded into the first scene's VBO, so if another
     *      Scene wants to use a certain mesh that's been loaded it cannot
     *      reference it since it is in another's.
     **/
    class IRONCLAD_API CScene
    {
    public:
        /**
         * Constructor for the scene.
         *  Since a single frame can have multiple scenes, it is necessary
         *  to specify how often the scene will have objects added to it.
         *  If you expect to add a variety of objects to a scene often, such
         *  as if you have a level with an extremely large variety of enemies,
         *  you should pass gfx::IC_DYNAMIC_SCENE. If you have all of the
         *  instances loaded a single time at the creation of the scene
         *  (before any calls to CScene::Render()) simply pass IC_STATIC_SCENE
         *  on creation. 
         *  All these flags really do is hint to the VBO how often it should
         *  expect to have vertex data added to the buffer.
         *
         * @param   CWindow&    Current window, for projection matrix
         * @param   SceneType   How often will the scene be changed?
         **/
        CScene(CWindow& Window, const SceneType scene_type);
        CScene(const uint16_t w, const uint16_t h,
               const math::matrix4x4_t& proj, const SceneType scene_type);
        ~CScene();

        /**
         * Initializes the scene.
         *  In this method, the default shader is loaded.
         *
         * @param   TRUE if shader loaded, FALSE if not.
         **/
        bool Init();

        /**
         * Adds an entity to be rendered in the current scene.
         *  This method will load the mesh (if it doesn't exist), and
         *  will create an instance of it at the specified position.
         * 
         * @param   std::string     Filename of mesh to load
         * @param   math::vector2_t Position to place the instance
         * @param   bool            Create a obj::CAnimation (optional=false)
         * @param   bool            Create a obj::CRigidBody (optional=false)
         * 
         * @return  The created obj::CEntity* on success, NULL on error.
         *          You may cast this value to obj::CAnimation / obj::CRigidBody
         *          provided that you are SURE you passed the proper
         *          parameters.
         **/
        obj::CEntity* AddMesh(const std::string& filename, const math::vector2_t& Pos,
            bool anim = false, bool rigid = false);
        obj::CEntity* AddMesh(asset::CMesh* pMesh, const math::vector2_t& Pos);
        inline void AddMesh(asset::obj::CEntity* pEntity)
        { mp_sceneObjects.push_back(pEntity); }

        /**
         * Adds an entity to be rendered in a specific order in the scene.
         *  This method will insert an entity (provided it loaded
         *  successfully) into the drawing queue at a certain position.
         *  If the provided position does not exist, the entity will be
         *  added at the end, just like CScene::AddMesh. 
         *  Index data can be retrieved from the queue using
         *  GetQueuePosition() or by searching yourself through the vector
         *  returned by GetObjects().
         *  
         * @param   uint16_t        Position to insert entity at
         * @param   std::string     Filename of mesh to load
         * @param   math::vector2_t Position to place the instance
         * @param   bool            Create a obj::CAnimation (optional=false)
         * @param   bool            Create a obj::CRigidBody (optional=false)
         * 
         * @return  Entity that was inserted, NULL if failed to load.
         * 
         * @see     CScene::GetQueuePosition()
         * @see     CScene::GetObjects()
         **/
        obj::CEntity* InsertMesh(const uint16_t position, 
            const std::string& filename, const math::vector2_t& Position,
            bool animate = false, bool physical = false);
        obj::CEntity* InsertMesh(const uint16_t position,
            asset::CMesh* pMesh, const math::vector2_t& Pos,
            bool animate = false, bool physical = false);
        bool InsertMesh(const uint16_t position, obj::CEntity* pEntity);

        /**
         * Deletes an existing mesh entity from the scene.
         *  This, as well as CScene::InsertMesh() should be used sparingly,
         *  and preferably prior to the main game loop, since the
         *  underlying container is an std::vector.
         *
         * @param   obj::CEntity*    Entity to remove
         * 
         * @bool    TRUE if exists and removed, FALSE otherwise.
         **/
        bool RemoveMesh(const obj::CEntity* pEntity)
        {
            int pos = this->GetQueuePosition(pEntity);
            if(pos == -1) return false;

            mp_sceneObjects.erase(mp_sceneObjects.begin() + pos);
            return true;
        }
        
        /**
         * Adds a pre-loaded light to the scene. 
         *  All of the parameters for the light should be set before being
         *  added to the scene, or at least prior to rendering.
         *  There may be a need to cast shadows in a scene, this should
         *  be specified when passing the light as well.
         *  
         * @param   CLight*     Light to render in the scene
         * 
         * @return  Accessible ID of the scene light.
         **/
        uint16_t AddLight(gfx::CLight* pLight);

        /**
         * Adds a material overlay to the scene.
         *  A material overlay is usually some sort of shader effect that
         *  takes place over the entire scene, rather than on some 
         *  individual instance.
         * 
         * @param   gfx::Effect*    The effect, properly set up
         * 
         * @return  TRUE if effect loaded successfully,
         *          FALSE on error in either.
         **/
        bool AddMaterialOverlay(CEffect* pEffect);

        /**
         * Removes an existing light from a scene.
         * @param   uint16_t    Light id
         * @return  TRUE if removed, FALSE if invalid id.
         **/
        bool RemoveLight(const uint16_t id);
        bool RemoveLight(const CLight* pLight);

        /**
         * Removes a post-processing effect from the scene.
         * @param   CEffect*    Effect to remove
         * @return  TRUE if removed, FALSE if not found.
         **/
        bool RemoveEffect(const gfx::CEffect* pEffect)
        {
            for(size_t i = 0; i < mp_sceneEffects.size(); ++i)
            {
                if(mp_sceneEffects[i] == pEffect)
                {
                    mp_sceneEffects.erase(mp_sceneEffects.begin() + i);
                    if(mp_sceneEffects.size() == 0) mp_sceneEffects.clear();
                    return true;
                }
            }

            return false;
        }

        /**
         * Pans the camera around the scene.
         *  This merely has the effect of adding a (x, y) vector to the
         *  position of the meshes when they are being rendered.
         *
         * @param    float    X-rate to pan at
         * @param    float    Y-rate to pan at
         * 
         * @see    MoveCamera()
         **/
        inline void PanCamera(const float dx, const float dy)
        { m_Camera.x += dx; m_Camera.y += dy; }

        inline void PanCamera(const math::vector2_t& dv)
        { this->PanCamera(dv.x, dv.y); }

        /**
         * Moves the camera to a certain position in the scene.
         * 
         * @param    float    X-position
         * @param    float    Y-position
         * 
         * @see    PanCamera()
         **/
        inline void MoveCamera(const float x, const float y)
        { m_Camera.x = x; m_Camera.y = y; }

        inline void MoveCamera(const math::vector2_t& Pos)
        { m_Camera = Pos; }

        /**
         * Renders the scene to the currently active frame buffer.
         *  If, for example, you wanted to render off-screen, you'd bind
         *  the other FBO before calling this.
         **/
        void Render();

        /**
         * Deletes all scene data.
         **/
        void Clear();

        void UpdateWindow(ic::gfx::CWindow& Window)
        {
            m_WindowDim = math::vector2_t(Window.GetW(), Window.GetH());
            m_WindowProj = Window.GetProjectionMatrixC();
            mp_Window = &Window;
        }

        /**
         * Toggles post-processing.
         * @return  What the value was originally, BEFORE toggling.
         **/
        inline bool TogglePostProcessing()
        { return !(m_postfx = !m_postfx); }

        /**
         * Toggles lighting.
         * @return  What the value was originally, BEFORE toggling.
         **/
        inline bool ToggleLighting()
        { return !(m_lighting = !m_lighting); }

        /**
         * Toggles wire-mesh rendering.
         * @return  What the value was originally, BEFORE toggling.
         **/
        inline bool ToggleWireMesh()
        {
            m_geo_type == GL_TRIANGLES  ?
            m_geo_type =  GL_LINE_STRIP :
            m_geo_type =  GL_TRIANGLES;

            return (m_geo_type == GL_TRIANGLES);
        }

        /**
         * Retrieves a light from the scene using a unique ID.
         *  Light IDs are given when adding lights to scene.
         *  
         * @param   uint16_t    ID of light
         * 
         * @return  Pointer to light object, NULL if out of range.
         **/
         inline CLight* GetLightFromScene(const uint16_t id)
         { return mp_sceneLights.size() < id ? NULL : mp_sceneLights[id]; }

         inline const CWindow* GetWindow() const 
         { return mp_Window; }

         /**
          * Returns the position in the object queue of the object.
          * 
          * @param  obj::CEntity*    Object to search for
          * 
          * @return -1 if object is not in queue, proper index otherwise.
          **/
         int GetQueuePosition(const obj::CEntity* pEntity) const;

         inline void QueryCamera(math::vector2_t& Position) const 
         { Position = m_Camera; }

         /**
          * Retrieves all of the lights in a scene.
          **/
         inline std::vector<CLight*>& GetLights()
         { return mp_sceneLights; }

         inline const std::vector<obj::CEntity*>& GetObjects() const
         { return mp_sceneObjects; }

         inline CVertexBuffer& GetGeometryBuffer()
         { return m_GeometryVBO; }

         friend class CLevel;

    private:

        /**
         * Renders a mesh surface.
         *  This method will bind the texture, then bind either the
         *  material shader or the default shader if the material's does
         *  not exist.
         *  Returns without rendering if the mesh or material is NULL,
         *  or if the model view uniform cannot be found within the
         *  material's shader.
         *
         * @param   surface_t*      Mesh instance to render indices from
         * @param   matrix4x4_t&    Position matrix of mesh surface
         * 
         * @pre     VBO must be bound.
         * @see     PostFXRender()
         */
        void StandardRender(
            gfx::surface_t* pSurface,
            const math::matrix4x4_t& ModelView);

        void StandardRender(obj::CEntity* pEntity,
            const math::matrix4x4_t& ModelView);

        /**
         * Renders lights on top of the entire scene.
         *  When a scene has multiple lights acting on everything,
         *  this method will be called for each light after a
         *  StandardRender is performed.
         *  Rendering is simply done using additive blending.
         *
         * @param   CLight*     Light to add to the scene
         **/
        void LightRender(gfx::CLight* pEffect);

        /**
         * Renders post-processing effects on top of the entire scene.
         *  When a scene has multiple effects acting on everything,
         *  this method will be called for each effect after all other 
         *  rendering has been performed.
         *  Rendering is done by ping-pong'ing between frame-buffers.
         *
         * @param   CEffect*        Effect to render over the scene
         * @param   CFramebuffer    Target to render scene onto
         * @param   uint32_t        Texture containing current scene
         * 
         * @return  The last FBO texture that was rendered onto.
         **/
        uint32_t PostProcessingRender(gfx::CEffect* pEffect,
            gfx::CFrameBuffer& Target, uint32_t source);

        /**
         * Renders shadow geometry.
         *    This is called internally every frame. 
         **/
        void ShadowRender();

        /**
         * Updates shadow casting vertex positions.
         *  This is called internally every frame. The shadow is drawn
         *  later on the GPU, this pushes new vertices to the VBO.
         **/
        void UpdateShadows(const math::vector2_t& LightPosition);

        static material_t       m_ShadowShader;

        CWindow*                mp_Window;
        CVertexBuffer           m_GeometryVBO, m_ShadowVBO;
        CFrameBuffer            m_FBO, m_FBOSwap;

        math::vector2_t         m_Camera, m_WindowDim;
        math::matrix4x4_t       m_WindowProj;

        std::vector<obj::CEntity*>   mp_sceneObjects;
        std::vector<CEffect*>   mp_sceneEffects;
        std::vector<CLight*>    mp_sceneLights;
        std::vector<vertex2_t>  m_shadowVertices;
        std::vector<uint16_t>   m_shadowIndices;

        uint32_t m_geo_type;
        bool m_lighting, m_postfx;
    };

}   // namespace gfx
}   // namespace ic

#endif // IRON_CLAD__GRAPHICS__SCENE_HPP

/** @} **/
