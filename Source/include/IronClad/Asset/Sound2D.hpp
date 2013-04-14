/**
 * @file
 *  Asset/Sound2D.hpp - Describes a sound effect in 2D space.
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

#include "IronClad/Math/Math.hpp"
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

        /**
         * Loads an audio file into memory.
         *  The lib-vorbis SDK is used to determine whether or not the
         *  given filename refers to a valid .ogg file or not.
         *  If it is determined NOT to be an .ogg file, it is assumed
         *  to be a .wav file and loaded as such, returning errors on
         *  any failures. 
         *
         * @param   std::string Filename to load
         * 
         * @return  TRUE if successfully loaded, FALSE otherwise.
         *          The actual error code can be checked by calling GetLastError().
         *
         * @todo    Add the ability to stream large .ogg files.
         **/
        bool LoadFromFile(const std::string& filename);
        bool LoadFromFile(const char* p_filename);

        /**
        * Loads audio from an existing CSound2D class.
        *  This method is pretty untested, I wouldn't really recommend
        *  using it. It takes the buffer and source ID's from the
        *  provided, existing CSound2D parameter.
        *
        * @param   CSound2D*   The audio to copy.
        * 
        * @return  Always TRUE.
        **/
        bool LoadFromAudio(const CSound2D* const p_Copy);

        /**
         * Moves the sound source to a new location.
         *  Because OpenAL automatically handles sound volume based on
         *  location to the listener, it may sometimes be necessary to
         *  move the sound source closer or farther to the listener.
         *  This moves it at a certain rate, not to a certain location.
         *  YOu can also manually use SetVolume() to alter the gain.
         *
         * @param   math::vector2_t& Rate at which to move sound source.
         * 
         * @see     SetVolume()
         **/
        void Move(const math::vector2_t& Rate);
        
        /**
         * Plays a loaded audio buffer.
         *  Due to the somewhat complicated source-management system in
         *  place, buffers are bound to an OpenAL source on demand.
         *  So whenever this method is called, an available source is
         *  found, and then the buffer is bound to it, and the audio
         *  is played. The CAssetManager is responsible for unloading the
         *  source after playback has completed in full.
         *  
         * @return  TRUE if the sound played, FALSE if not.
         **/
        bool Play();

        /**
         * Pauses current audio buffer.
         *  The buffer maintains linked to its source despite being paused,
         *  and will only be deleted if the source status is CSTOPPED.
         *  
         * @return  TRUE if paused, FALSE if nothing loaded.
         **/
        bool Pause();

        /**
         * Stops the audio from playing.
         *  Audio looping is always enabled, with no method (currently)
         *  to disable it. This is due to the fact that I've never /not/
         *  needed to disable looping.
         *
         * @return  TRUE if stopped, FALSE if nothing is loaded.
         * 
         * @see     asset::C2DSound::Play()
         * @todo    Add looping capabilities (or lack there-of).
         **/
        bool Stop();

        /**
         * Unloads the source being used for playback.
         *  Since OpenAL is limited to 256 audio sources, it is important
         *  to be conservative with the available resources.
         *  Though I doubt all 256 will ever be used simultaneously, a
         *  system is in place to minimize source-hogging. This method
         *  should only be called by an CAssetManager after checking
         *  that playback has been completed. 
         * 
         * @return  TRUE if the source was unloaded successfully,
         *          FALSE if not, or no source loaded.
         **/
        bool UnloadSource();

        inline void SetVolume(float v) { m_volume = v; }
        
        void SetPosition(const math::vector2_t& Position);
        void SetVelocity(const math::vector2_t& Velocity);
        void SetDirection(const math::vector2_t& Direction);

        int     GetAudioState() const;
        ALenum  GetLastError() const;
        ALuint  GetBuffer() const;
        ALuint  GetSource() const;
        ALuint  GetSourceIndex() const;
        float   GetVolume() const;
        const std::string& GetError() const;

        /// The CAssetManager is the only thing capable of loading audio.
        friend class CAssetManager;

        /// Adjusts volume for all of the loaded sounds.
        static void AdjustVolume(const float dv);

        /// All of the loaded sounds.
        static std::vector<CSound2D*> s_allSounds;

    private:
        /// Finds the next available OpenAL source.
        static void GetAvailableSource();

        /// Constructs an instance of the CSound2D class.
        CSound2D(bool orig = false, const void* const own = NULL);

        /**
         * Loads a .wav file.
         *  This is only called by LoadFromFile() after determining that
         *  the given data is not a valid Ogg-Vorbis audio file.
         *
         * @param   char*   Filename
         * @return  TRUE on successful load, FALSE otherwise.
         **/
        bool LoadFromFile_WAV(const char* p_filename);

        std::string m_error;

        ALuint  m_buffer;
        ALint   m_source;
        ALenum  m_lasterror;
        ALfloat m_volume;

        bool    m_loop;
    };
}   // namespace asset
}   // namespace ic

#endif // IRON_CLAD__SOUND_2D_HPP

/** @} **/
 