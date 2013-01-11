/**
 * @file
 *    Utils/Helper.cpp - A collection of helper functions for string
 *    manipulation and other procedures.
 *
 * @author      George Kudrayvtsev (switch1440)
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

    static bool CheckCollision(const math::rect_t& oner, const math::rect_t& two)
    {
        int top2, bottom2, right2, left2;

        left2   = two.x;
        right2  = two.x + two.w;
        top2    = two.y;
        bottom2 = two.y + two.h;

        if(oner.y + oner.h < top2)
            return false;
        if(oner.y > bottom2)
            return false;
        if(oner.x + oner.w < left2)
            return false;
        if(oner.x > right2)
            return false;

        return true;
    }

    static bool CheckCollision(const math::vector2_t& one, const math::rect_t& two)
    {
        return CheckCollision(math::rect_t(one.x, one.y, 1, 1), two);
    }

    static bool CheckCollision(const math::vector2_t& one, const math::vector2_t& two)
    {
        return CheckCollision(one, math::rect_t(two.x, two.y, 1, 1));
    }

}   // namespace util
}   // namespace ic

#endif // IRON_CLAD__UTILS__HELPER_HPP

/** @} **/
