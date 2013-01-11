/**
 * @file
 *    Timer.hpp - Designed for easy frame-rate control during rendering operations.
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
 * @addtogroup Engine
 * @{
 **/

#ifndef IRON_CLAD__TIMER_HPP
#define IRON_CLAD__TIMER_HPP

#include "Base/Types.hpp"
#include "Graphics/Window.hpp"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif // _WIN32

namespace ic
{
namespace util
{
    inline void IRONCLAD_API sleep(const double ms)
#ifdef _WIN32
    { Sleep(ms); }
#else
    { usleep(ms); }
#endif // _WIN32

    /**
    * Provides an interface for tracking time.
    *  Common uses would be frame-rate management or recording the 
    *  length of time that passes between the start and end of a frame.
    *  
    * @todo    There are some errors with regulating frame-rate. 
    *            In gDEBugger, it shows as maxing out at 45fps when set to 60fps.
    **/
    class IRONCLAD_API CTimer
    {
    public:
        CTimer(const int fps = 60) : m_framerate(fps) {}
        ~CTimer(){}

        /**
        * Starts the timer.
        *  This should be called before performing any operation which
        *  you would like to record execution speed on. 
        *
        * @see CTimer::GetElapsed()
        **/
        void Start()
        {
            m_started = glfwGetTime();
        }

        /**
        * Retrieves the total elapsed time since Start() was called.
        * 
        * @pre     Start() was called.
        * @return  Elapsed time.
        **/
        double GetElapsed()
        {
            double old_time = m_started;
            m_started = glfwGetTime();
            return m_started - old_time;
        }

        /**
        * Pauses the program for some time to regulate frame rate.
        *    This method should be called at the end of a frame after
        *    all other operations have been completed in order to 
        *    maintain a consistent frame rate.
        **/
        void Regulate()
        {
            double elapsed = this->GetElapsed();

            if(elapsed < 1000.0 / m_framerate)
                sleep(1000.0 / m_framerate - elapsed);
        }

        inline int GetFramerate() const
        { return m_framerate; }

        inline void SetFramerate(const int fps)
        { m_framerate = fps; }

    private:
        double m_elapsed, m_started;
        int m_framerate;
    };
}   // namespace util
}   // namespace ic

#endif // IRON_CLAD__TIMER_HPP

/** @} **/