/**
 * @file
 *	Audio/MusicPlayer.hpp - A class acting as a sequential music track player.
 *
 * @author      George Kudrayvtsev (halcyon)
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
 * @addtogroup Audio
 * @{
 **/

#ifndef IRON_CLAD__MUSIC_PLAYER_HPP
#define IRON_CLAD__MUSIC_PLAYER_HPP

#include "Asset/Sound2D.hpp"
#include "Asset/AssetManager.hpp"

namespace ic
{
namespace audio
{
    class IRONCLAD_API CMusicPlayer
    {
    public:
        CMusicPlayer() : mp_CurrentSong(NULL), m_index(0) {}
        ~CMusicPlayer();

        bool AddSongToQueue(const char* pfilename);
        void PurgeQueue();

        void Play();
        void Pause();
        void Stop();

        void Update();

    private:
        bool NextSong();

        std::vector<asset::CSound2D*> mp_allSongs;
        asset::CSound2D* mp_CurrentSong;
        uint32_t m_index;
    };
}   // namespace asset
}   // namespace ic

#endif // IRON_CLAD__MUSIC_PLAYER_HPP

/** @} **/
