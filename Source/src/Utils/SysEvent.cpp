#include "Utils/SysEvent.hpp"

using namespace ic;
using util::CEventHandler;
using util::EventType;

util::SystemEvent               CEventHandler::ActiveEvent;
std::vector<util::SystemEvent>  CEventHandler::pollableEvents;
bool                            CEventHandler::pending;

util::SystemEvent::SystemEvent()
{
    Reset();
}

util::SystemEvent::SystemEvent(const util::SystemEvent& Copy)
{
    type    = Copy.type;
    mpos    = Copy.mpos; 
    mbutton = Copy.mbutton;
    symkey  = Copy.symkey;
    key     = Copy.key;
}

void util::SystemEvent::Reset()
{
    type    = IC_NO_EVENT;
    mpos    = math::vector2_t(-1, -1);
    mbutton = IC_BUTTON_UNKNOWN;
    symkey  = IC_KEY_UNKNOWN;
    key     = '\0';
}

CEventHandler::~CEventHandler()
{
}

CEventHandler& CEventHandler::GetInstance()
{
    static CEventHandler g_EventHandler;
    CEventHandler::pending = false;

    return g_EventHandler;
}

bool CEventHandler::PopEvent(util::SystemEvent& Evt)
{
    if(pollableEvents.size() == 0) return false;
    
    Evt = pollableEvents.back();
    pollableEvents.erase(pollableEvents.end() - 1);
    
    return true;
}

int CEventHandler::WindowCloseCallback()
{
    ActiveEvent.type = IC_QUIT;

    pollableEvents.push_back(ActiveEvent);
    ActiveEvent.Reset();
    pending = true;

    return GL_TRUE;
}

void CEventHandler::KeyboardCallback(const int key, const int action)
{
    // If the key is in the ASCII range, set it to the printable
    // character in the event structure.
    if(key >= IC_KEY_SPACE && key < 128)
        ActiveEvent.key = (char)key;

    // Not printable, so set it to the symbol key.
    else
        ActiveEvent.symkey = key;

    ActiveEvent.type = (action == GLFW_PRESS) ? IC_KEYDOWN : IC_KEYUP;

    pollableEvents.push_back(ActiveEvent);
    ActiveEvent.Reset();
    pending = true;
}

void CEventHandler::MouseCallback(const int button, const int action)
{
    ActiveEvent.mbutton = button;
    ActiveEvent.mpos = util::GetMousePosition();
    ActiveEvent.type = (action == GLFW_PRESS) ? IC_MOUSEDOWN : IC_MOUSEUP;

    pollableEvents.push_back(ActiveEvent);
    ActiveEvent.Reset();
    pending = true;
}

void CEventHandler::MousePosCallback(const int x, const int y)
{
    ActiveEvent.type    = IC_MOUSEMOVE;
    ActiveEvent.mpos.x  = x;
    ActiveEvent.mpos.y  = y;

    pollableEvents.push_back(ActiveEvent);
    ActiveEvent.Reset();
    pending = true;
}

math::vector2_t util::GetMousePosition()
{
    int x, y;
    glfwGetMousePos(&x, &y);
    return math::vector2_t(x, y);
}

void util::GetMousePosition(int& x, int& y)
{
    glfwGetMousePos(&x, &y);
}

int util::GetKeyState(const int key)
{
    return glfwGetKey(key) == GLFW_PRESS ? IC_KEYDOWN : IC_KEYUP;
}

int util::GetMouseState(const ButtonCode button)
{
    return (glfwGetMouseButton(button) == GLFW_PRESS) ? 
        IC_MOUSEDOWN : IC_MOUSEUP;
}
