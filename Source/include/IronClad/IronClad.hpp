/**
 * @file
 *  IronClad.hpp - Includes all files from the IronClad engine
 *  and functions for initialization.
 *
 * @author      George Kudrayvtsev (halcyon)
 * @version     1.0
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
 * @addtogroup Engine
 *  Contains high-level functionality for manipulating scenes, effects,
 *  and other abstracted rendering operations.
 * 
 * @{
 **/

/**
 * TODO LIST:
 * 
 * @todo    Geometry shader.
 * @todo    Shadows.
 * @todo    Make CFont inherit CAsset.
 * 
 **/

#ifndef IRON_CLAD_HPP
#define IRON_CLAD_HPP

// Link libraries.
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "GLEW32.lib")
#pragma comment(lib, "GLFW.lib")

#pragma comment(lib, "freetype.lib")
#pragma comment(lib, "OpenAL32.lib")
#pragma comment(lib, "alut.lib")
#pragma comment(lib, "libvorbisfile.lib")

#include <cstdlib>
#include <ctime>

// Include high-level engine interfaces.
#include "IronClad/Math/Math.hpp"

#include "IronClad/Base/Errors.hpp"
#include "IronClad/Base/Types.hpp"

#include "IronClad/Asset/AssetManager.hpp"

#include "IronClad/Graphics/Globals.hpp"
#include "IronClad/Graphics/Effect.hpp"
#include "IronClad/Graphics/Light.hpp"
#include "IronClad/Graphics/Window.hpp"
#include "IronClad/Graphics/Scene.hpp"

#include "IronClad/Entity/RigidBody.hpp"
#include "IronClad/Entity/Animation.hpp"
#include "IronClad/Entity/QuadTree.hpp"

#include "IronClad/Audio/MusicPlayer.hpp"
#include "IronClad/Utils/Utilities.hpp"

#include "IronClad/GUI/Font.hpp"
#include "IronClad/GUI/Menu.hpp"

#include "IronClad/Level.hpp"

namespace ic
{
    IRONCLAD_API bool Init();
    IRONCLAD_API void Quit();
}   // namespace ic

#endif // IRON_CLAD_HPP

/** @} **/
