/**
 * @file
 *  IronClad.cpp - Contains definitions for the engine initialization
 *  and release functions.
 *
 * @author      George Kudrayvtsev (halcyon)
 * @version     0.1
 * @copyright   Apache License v2.0
 *  Licensed under the Apache License, Version 2.0 (the "License").\n
 *  You may not use this file except in compliance with the License.\n
 *  You may obtain a copy of the License at:
 *  http://www.apache.org/licenses/LICENSE-2.0 \n
 *  Unless required by applicable law or agreed to in writing, software\n
 *  distributed under the License is distributed on an "AS IS" BASIS,\n
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\n
 *  See the License for the specific language governing permissions and\n
 *  limitations under the License.
 *
 * @mainpage IronClad Engine Documentation
 * 
 * @section use Usage
 *  The IronClad engine is only compatible with Windows. If you downloaded
 *  the binary package, you should add the include/IronClad directory
 *  to your include path.
 *  Then, you will need to link the lib/IronClad.lib library in your 
 *  project, as well as have the lib/IronClad.dll in the same folder as
 *  your executable file.
 *  To use the engine in your source code, simply include IronClad.hpp.
 * 
 * @section src_note Source code notes
 *  Since structs and classes in C++ are basically identical, throughout
 *  IronClad, structs are used in cases where all internal variables are
 *  publicly accessible, such as the various math-related types.
 *  Also, all structures are followed by  a _t suffix for easy
 *  distinguishing and type creation.
 **/

#include "IronClad.hpp"

bool ic::Init()
{
    ic::util::g_Log.Flush();
    ic::util::g_Log << "[INFO] Initializing IronClad engine.\n";
    ic::util::g_Log.PrintLastLog();

    // Seed the RNG.
    srand((uint32_t)time(NULL));

    // Initialize FreeType2 library.
    ic::util::g_Log.Flush();
    ic::util::g_Log << "[INFO] Initializing FreeType: ";
    if(!ic::gui::CFont::Initialize())
    {
        ic::util::g_Log << "failure.\n";
        return false;
    }
    ic::util::g_Log << "success.\n";
    ic::util::g_Log.PrintLastLog();

    // Initialize OpenAL.
    ic::util::g_Log.Flush();
    ic::util::g_Log << "[INFO] Initializing OpenAL:   ";
    if(!ic::asset::CSound2D::InitializeOpenAL())
    {
        ic::util::g_Log << "failure.\n";
        return false;
    }
    ic::util::g_Log << "success.\n";
    ic::util::g_Log.PrintLastLog();

    // Initialize GLFW.
    ic::util::g_Log.Flush();
    ic::util::g_Log << "[INFO] Initializing GLEW:     ";
    if(glfwInit() == GL_FALSE)
    {
        ic::util::g_Log << "failure.\n";
        return false;
    }
    ic::util::g_Log << "success.\n";
    ic::util::g_Log.PrintLastLog();

    ic::util::g_Log << "[INFO] Debug build:           ";
#ifdef _DEBUG
    ic::util::g_Log << "true.\n";
#else
    ic::util::g_Log << "false.\n";
#endif // _DEBUG
    ic::util::g_Log.PrintLastLog();

    return true;
}

void ic::Quit()
{
    ic::gui::CFont::DeInitialize();
    ic::asset::CAssetManager::DestroyAll();

    ic::util::g_Log.Flush();
    ic::util::g_Log << "[INFO] Quitting IronClad engine.\n";
    ic::util::g_Log.PrintLastLog();
    ic::util::g_Log.Close();

    glfwTerminate();
}
