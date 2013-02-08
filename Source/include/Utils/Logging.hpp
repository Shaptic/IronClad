/**
 * @file
 *    Utils/Logging.hpp - Declares the CLogging singleton class.
 *
 * @author      George Kudrayvtsev (switch1440)
 * @version     1.0
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
 * @addtogroup Utilities
 * Contains a collection of various helpful functions that are not related
 * to the primary engine in any way.
 *  
 * @{
 **/

#ifndef IRON_CLAD__UTILS__LOGGING_HPP
#define IRON_CLAD__UTILS__LOGGING_HPP

#include <ctime>

#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>

#ifdef _WIN32
  #define WIN32_LEAN_AND_MEAN
  //#include <Windows.h>
#else
  #include <iostream>
#endif // _WIN32

#include "Base/Types.hpp"

namespace ic
{

/**
 * Contains various utilities that can help with loading, rendering,
 * logging, and debugging.
 **/
namespace util
{
    /**
     * A singleton class that handles logging operations to a file
     * called LastRun.log.
     **/
    class IRONCLAD_API CLogging
    {
    public:
        virtual ~CLogging();

        /**
         * Logs given data.
         *  Due to its templated nature, and the versatility of
         *  std::ostream, anything can be passed as an argument.
         *  The given data is written to an std::stringstream, and is only
         *  writted to the file when CLogging::Flush() is called.
         *
         * @param template<class T> Data to log
         * @return A reference to the CLogging class so that streaming
         *  can be grouped together.
         **/
        template<class T> CLogging& operator<<(const T& data);

        /**
         * Closes the logging file.
         *  A CLogging::Flush() operation is performed before closing.
         **/
        void Close();

        /**
         * Writes pending data to the file.
         *  The std::stringstream is written to the logging file, then
         *  cleared. Thus, calls to GetLastLog() will return a blank string. 
         *  
         * @see CLogging::GetLastLog()
         **/
        void Flush();
        void PrintLastLog() const;
        std::string GetLastLog() const;

        /**
         * Sets the width of the next string, filling it with spaces.
         *  Works similarly to std::setw()
         *
         * @param   int     Length of string, -1 to reset
         **/
        CLogging& SetWidth(const int w);

        static CLogging& GetInstance()
        {
            static CLogging g_Log("Engine.log");
            return g_Log;
        }

    private:
        CLogging(const char* p_filename);
        CLogging(const CLogging& Log);
        void operator=(const CLogging& Log);

        std::stringstream m_log;
        std::ofstream m_file;
        int m_width;
    };

    template<class T> CLogging& CLogging::operator<<(const T& data)
    {
        m_log << std::setfill('0') << std::setw(m_width) << data;
        m_width = -1;
        return (*this);
    }

    /**
     * Provides global access to the singleton instance of the logger.
     **/
    static CLogging& g_Log = CLogging::GetInstance();

}   // namespace util
}   // namespace ic

#endif // IRON_CLAD__UTILS__LOGGING_HPP

/** @} **/