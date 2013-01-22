/**
 * @file
 *    Utils/Helper.cpp - A collection of helper functions for string
 *    manipulation and other procedures.
 *
 * @author      George Kudrayvtsev (switch1440)
 * @version     1.0.1
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
 * @addtogroup Utilities
 * @{
 **/

#ifndef IRON_CLAD__UTILS__HELPER_HPP
#define IRON_CLAD__UTILS__HELPER_HPP

#include "Base/Types.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace ic
{
namespace util
{
    IRONCLAD_API int  num_len(const int number);
    IRONCLAD_API void toupper(std::string& str);
    IRONCLAD_API std::string toupper_ret(const std::string& str);
    IRONCLAD_API std::string combine(const std::string& str1, const char* str2);
    IRONCLAD_API std::string combine(const char* str2, const std::string& str1);
    IRONCLAD_API std::vector<std::string> split(const std::string& str, char token);
}   // namespace util
}   // namespace ic

#endif // IRON_CLAD__UTILS__HELPER_HPP

/** @} **/
