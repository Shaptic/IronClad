/**
 * @file
 *	Asset/Sound2D.hpp - Describes a sound effect in 2D space.
 *
 * @author      George Kudrayvtsev (switch1440)
 * @version     1.0
 * @copyright   Apache License v2.0
 *  Licensed under the Apache License, Version 2.0 (the "License").			\n
 *  You may not use this file except in compliance with the License.		\n
 *  You may obtain a copy of the License at:
 *  http://www.apache.org/licenses/LICENSE-2.0 								\n
 *  Unless required by applicable law or agreed to in writing, software		\n
 *  distributed under the License is distributed on an "AS IS" BASIS,		\n
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\n
 *  See the License for the specific language governing permissions and		\n
 *  limitations under the License.
 *
 * @addtogroup Assets
 * @{
 **/

#ifndef IRON_CLAD__SOUND_2D_HPP
#define IRON_CLAD__SOUND_2D_HPP

#include <string>
#include <vector>

#include "AL/al.h"
#include "AL/alut.h"
#include "vorbis/vorbisfile.h"

#include "Math/Math.hpp"
#include "Asset.hpp"

namespace ic
{
namespace asset
{
    /// All of the sources.
    static ALuint   s_sources[256];

    /// The index of the currently available source.
    static int      s_available     = 0;

    /// Total amount of AL assets loaded.
    static uint32_t s_assetcount    = 0;

    class IRONCLAD_API CSound2D : public CAsset
    {
    public:
        ~CSound2D();

        /// Initializes OpenAL and ALUT.
        static bool InitializeOpenAL();

        inline bool LoadFromFile(const std::string& filename)
        {
            return this->LoadFromFile(filename.c_str());
        }

        bool LoadFromFile(const char* p_filename);
        bool LoadFromAudio(CSound2D* const p_Copy);
        bool LoadFromFile_WAV(const char* p_filename);

        /**
         * Moves the sound source to a new location.
         *  Because OpenAL automatically handles sound volume based on
         *  location to the listener, it may sometimes be necessary to
         *  move the sound source closer or farther to the listener.
         *  This moves it at a certain rate, not to a certain location.
         *
         * @param math::vector2_t& Rate at which to move sound source.
         **/
        void Move(const math::vector2_t& Rate);
        
        bool Play();
        bool Pause();
        bool Stop();
        bool UnloadSource();

        void SetPosition(const math::vector2_t& Position);
        void SetVelocity(const math::vector2_t& Velocity);
        void SetDirection(const math::vector2_t& Direction);

        inline void SetVolume(float v)
        {
            math::clamp<float>(v, 0.f, 1.f);
            m_volume = v;
        }

        int     GetAudioState() const;
        ALenum  GetLastError() const;
        ALuint  GetBuffer() const;
        ALuint  GetSource() const;
        ALuint  GetSourceIndex() const;

        /// The CAssetManager is the only thing capable of loading audio.
        friend class CAssetManager;

        static std::vector<asset::CSound2D*> mp_allSounds;

    private:
        /// Constructs an instance of the CSound2D class.
        CSound2D(bool orig);

        /// Finds the next available OpenAL source.
        static void GetAvailableSource();

        ALuint  m_buffer;
        ALint   m_source;
        ALenum  m_lasterror;
        ALfloat m_volume;
    };
}   // namespace asset
}   // namespace ic

#endif // IRON_CLAD__SOUND_2D_HPP

/** @} **/