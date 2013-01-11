/**
 * @file
 *	Base/Errors.hpp - Simple error-handling functions.
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
 * @addtogroup Engine
 * @{
 **/

#ifndef IRON_CLAD__BASE__ERRORS_HPP
#define IRON_CLAD__BASE__ERRORS_HPP

#include <string>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <cstdio>
#endif // _WIN32

namespace ic
{
    /**
     * Pops up a window with an error message.
     *  Platform-dependant, so far, a MessageBox pops up on Windows,
     *  whereas *nix systems will simply print to STDERR.
     * 
     * @param   std::string     Error message
     * @param   char*           Title               (optional="Fatal Error")
     * @param   bool            Quit after showing  (optional=true)
     **/
    inline void handle_error(
        const std::string& msg,
        const char* title = "Fatal Error.", 
        const bool quit = true)
    {
#ifdef _WIN32
        MessageBoxA(NULL, msg.c_str(), title, MB_OK);
#else
        fprintf(stderr, "[%s]: %s\n", title, msg);
#endif // _WIN32

        if(quit) exit(1);
    }
}

#endif // IRON_CLAD__BASE__ERRORS_HPP

/** @} **/