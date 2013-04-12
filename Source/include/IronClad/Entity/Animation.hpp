/**
 * @file
 *  Entity/Animation.hpp - A basic abstraction of animation functions like loading
 *  a sprite sheet and switching sprites at certain rates.
 *
 * @author      George Kudrayvtsev (halcyon)
 * @version     1.0.2
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
 * @{
 **/

#ifndef IRON_CLAD__ANIMATION_HPP
#define IRON_CLAD__ANIMATION_HPP

#include "IronClad/Utils/Utilities.hpp"
#include "IronClad/Graphics/Framebuffer.hpp"
#include "RigidBody.hpp"

namespace ic
{
namespace obj
{
    /**
     * An animate-able extension of the obj::CEntity class.
     *  This class acts exactly like an entity, but contains the ability
     *  to easily switch between sprites either automatically after a 
     *  certain period of time or manually.
     *  Currently, this is meant exclusively for simple, single-texture
     *  quad meshes, as that makes material binding much easier to 
     *  implement correctly.
     *  Another limitation is that all of the animations in the sprite
     *  sheet must be the exact same size.
     *  
     * @todo    Expand animation abilities to non-quad meshes.
     **/
    class IRONCLAD_API CAnimation : public CRigidBody
    {
    public:
        struct AnimationHeader
        {
            uint16_t width;         // 2 bytes * 4 fields = 8 bytes.
            uint16_t height;
            uint16_t columns;

            asset::CTexture*    pTexture;
            gfx::CMeshInstance* pMesh;
        };

        CAnimation() : m_last(util::CTimer::GetTimeElapsed()),
                       m_delay(1.f), m_loops_done(0) {}

        virtual ~CAnimation()
        {
            asset::CAssetManager::Destroy(m_SheetDetails.pTexture);
        }

        /**
         * Load a custom animation file.
         *  This will load an .icanim image file, which internally
         *  specifies sprite dimensions in the sheet.
         * 
         * @param   std::string         Filename
         * @param   gfx::CVertexBuffer  Vertex buffer to load quad-mesh into
         * 
         * @return  TRUE on success, FALSE on failure
         * @see     Docs/ICAnim.spec
         **/
        bool LoadFromFile(const std::string& filename,
                          gfx::CVertexBuffer& VBO);

        /**
         * Replaces the current texture and dimensions.
         *  This will pass new texture coordinate offsets to the fragment shader,
         *  as well as give the renderer a new texture to work with.
         *  At times, this causes a single frame of texture artifacts, for 
         *  which the cause is currently unknown.
         * 
         * @param   AnimationHeader Header containing texture / dimension data
         **/
        void SwapSpriteSheet(const AnimationHeader& Header);
        void SwapSpriteSheet(const CAnimation* pAnimation);

        /**
         * Toggles animation.
         *  A obj::CAnimation with animation disabled acts exactly like a 
         *  obj::CEntity for all intents and purposes. Disabling animation
         *  leaves the currently active texture as the one rendered,
         *  as opposed to the original texture.
         *
         * @param   bool    Enable / Disable animation
         **/
        void EnableAnimation(bool flag)
        {
            m_enabled = flag;
        }

        /**
         * Sets the rate of animation.
         *  The internal sprites will iterate as this limit is reached.
         *  It is absolutely essential that you call Update() at the end
         *  of each frame in order to track this value properly.
         **/
        void SetAnimationRate(const float delta);

        /**
         * Iterates the current texture to the next one in the queue.
         * @return  TRUE if iterated to next one,
         *          FALSE if looping to beginning.
         **/
        bool NextSprite();

        /**
         * Iterates the current texture to the previous one in the queue.
         * @return  TRUE if iterated to previous one,
         *          FALSE if looping to end.
         **/
        bool PrevSprite();

        /**
         * Updates the current time delta value.
         *  This method will accurately measure time elapsed between 
         *  texture switches and will switch accordingly if necessary.
         **/
        virtual void Update();

        uint32_t GetLoopCount() const
        { return m_loops_done; }

        uint16_t GetAnimationCount() const 
        { return m_SheetDetails.columns; }

        const AnimationHeader& GetHeader() const
        { return m_SheetDetails; }

        AnimationHeader& GetHeader()
        { return m_SheetDetails; }

        uint8_t GetAnimationIndex() const
        { return m_active + 1; }

        void SetAnimation(const uint8_t index);

    protected:
        AnimationHeader m_SheetDetails;     // Internal sprite sheet details
        uint8_t         m_active;           // Currently active sprite
        float           m_TexcDim;          // Calculated texture width
        float           m_delay;            // Delay between texture swaps
        float           m_last;             // Time since last Next/PrevSprite() call
        bool            m_enabled;          // Is animation enabled?
        int             m_tc_loc, m_tc_str; // Shader uniform locations
        int             m_loops_done;       // Loops completed
    };
}
}

#endif // IRON_CLAD__ANIMATION_HPP

/** @} **/