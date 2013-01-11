/**
 * @file
 *    Utils/SysEvent.hpp - Defines the CEvent structure, containing all of the
 *    options for a system event in IronClad.
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

#ifndef IRON_CLAD__UTILS__SYS_EVENT_HPP
#define IRON_CLAD__UTILS__SYS_EVENT_HPP

#include <vector>

#include "Base/Types.hpp"
#include "Graphics/Window.hpp"

namespace ic
{
namespace util
{
    /**
     * Describes the types of events handled by the IronClad engine.
     **/
    enum IRONCLAD_API EventType
    {
        IC_NO_EVENT = -1,
        IC_QUIT,            // Quitting the window
        IC_RESIZE,          // Resizing the window
        IC_MINIMIZE,        // Minimizing the window
        IC_MAXIMIZE,        // Maximizing the window
        IC_KEYDOWN,         // Keyboard key pressed
        IC_KEYUP,           // Keyboard key released
        IC_MOUSEDOWN,       // Mouse button pressed
        IC_MOUSEUP,         // Mouse button released
        IC_MOUSEMOVE,       // Mouse moved
        IC_EVENT_COUNT      // Total number of events (unused)
    };

    /**
     * Structure containing all elements of a system event in IronClad.
     **/
    struct IRONCLAD_API SystemEvent
    {
        SystemEvent();
        SystemEvent(const SystemEvent& Copy);

        /**
         * Resets all event attributes to default (invalid) values.
         **/
        void Reset();

        /** Mouse position. **/
        math::vector2_t mpos;

        /** The type of event. **/
        EventType       type;

        /** Mouse button. **/
        int             mbutton;

        /** Keyboard symbol key. **/
        int             symkey;

        /** Printable keyboard character. **/
        char            key;
    };

    /**
     * Singleton class responsible for managing a queue of system events.
     **/
    class IRONCLAD_API CEventHandler
    {
    public:
        virtual ~CEventHandler();

        static CEventHandler& GetInstance();

        /**
         * Retrieves the latest event from the global queue.
         *  This event is permanently removed from the queue after 
         *  this method is called.
         *  
         * @param   SystemEvent&    The structure to load latest data into
         * 
         * @return  Whether or not there are any events left in the queue.
         **/
        static bool PopEvent(SystemEvent& Evt);

        /**
         * Callback functions for GLFW.
         **/
        static int  WindowCloseCallback();
        static void KeyboardCallback(const int key, const int action);
        static void MouseCallback(const int button, const int action);
        static void MousePosCallback(const int x, const int y);

    private:
        CEventHandler(){}
        CEventHandler(const CEventHandler&);
        CEventHandler& operator=(const CEventHandler&);

        static SystemEvent ActiveEvent;
        static std::vector<SystemEvent> pollableEvents;
        static bool pending;
    };

    /**
     * Retrieves the current mouse position.
     * @return  Mouse position, of course.
     **/
    math::vector2_t IRONCLAD_API GetMousePosition();

    /**
     * Loads current mouse position into given variables.
     * @param   int&    X-coordinate
     * @param   int&    Y-coordinate
     **/
    void IRONCLAD_API GetMousePosition(int& x, int& y);

    /**
     * Retrieves the state of a certain key.
     *  Acceptable keys are uppercase printable characters ('A', '#', etc.)
     *  as well as GLFW key constants such as GLFW_KEY_LSHIFT.
     *  
     * @param   int     Key to test state for
     * 
     * @return  IC_KEYDOWN if the key is currently being pressed,
     *          IC_KEYUP if it is not.
     *  
     * @todo    Make IronClad specific constants for non-printable chars.
     **/
    int IRONCLAD_API GetKeyState(const int key);

    /**
     * Retrieves the state of a certain mouse button.
     *  Acceptable buttons are specified by GLFW mouse button constants
     *  GLFW_MOUSE_BUTTON_1 through 8.
     * 
     * @param   int     Mouse button to test state for
     * 
     * @return  IC_MOUSEDOWN if button is pressed, 
     *          IC_MOUSEUP if it is not.
     *          
     * @todo    Make IronClad specific mouse button constants.
     **/
    int IRONCLAD_API GetMouseState(const int button);

    /**
     * Globally accessible CEventHandler singleton instance.
     **/
    static CEventHandler& g_EventQueue = CEventHandler::GetInstance();

}   // namespace util
}   // namespace ic

#endif // IRON_CLAD__UTILS__SYS_EVENT_HPP

/** @} **/