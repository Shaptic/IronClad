#include "Graphics/Window.hpp"

using namespace ic;

using gfx::CWindow;
using util::g_Log;

math::matrix4x4_t CWindow::m_ProjectionMatrix;

CWindow::CWindow() : m_width(0),
    m_height(0), m_fullscreen(0) {}

CWindow::~CWindow() {}

/**
 * @todo    Make the window context consistent with the
 *          IRONCLAD_MINIMUM_GL_VERSION constant.
 **/
bool CWindow::Create(
    const uint16_t  width,
    const uint16_t  height,
    const char*     ptitle)
{
    m_width     = width;
    m_height    = height;

    // Full screen in Release builds.
#ifdef _DEBUG
    m_fullscreen= false;
#else
    m_fullscreen= false;//true;
#endif // _DEBUG

    // Create the window.
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    if(glfwOpenWindow(m_width, m_height, 8, 8, 8, 8, 24, 8,
        m_fullscreen ? GLFW_FULLSCREEN : GLFW_WINDOW) == GL_FALSE)
    {
        g_Log.Flush();
        g_Log << "[ERROR] Failed to create OpenGL window!" << ".\n";
        g_Log.PrintLastLog();

        handle_error(g_Log.GetLastLog());
        return false;
    }

    // Disable automatic event handling.
    //glfwDisable(GLFW_AUTO_POLL_EVENTS);

    // Set the window caption.
    glfwSetWindowTitle(ptitle);

    // Log some important OpenGL data.
    int majv, minv;
    glfwGetGLVersion(&majv, &minv, NULL);
    const char* pversion    = (char*)glGetString(GL_VERSION);
    const char* pglslv      = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
    const char* pvendor     = (char*)glGetString(GL_VENDOR);
    const char* prenderer   = (char*)glGetString(GL_RENDERER);

    g_Log.Flush();
    g_Log << "[INFO] OpenGL version : " << pversion     << "\n";
    g_Log << "[INFO] GLSL version   : " << pglslv       << "\n";
    g_Log << "[INFO] OpenGL vendor  : " << pvendor      << "\n";
    g_Log << "[INFO] OpenGL renderer: " << prenderer    << "\n";
    g_Log.PrintLastLog();

    // Check for a supported OpenGL version.
    if(majv + minv / 10.0f < IRONCLAD_MINIMUM_GL_VERSION)
    {
        g_Log.Flush();
        g_Log << "[ERROR] OpenGL version unsupported.\n";
        g_Log << "[ERROR] Please update your video card drivers.\n";
        g_Log.PrintLastLog();
        handle_error(g_Log.GetLastLog());
        return false;
    }

    // Set up a 2D perspective.
    glClearColor(0, 0, 0, 1);
    glViewport(0, 0, m_width, m_height);
    
    // Enable some OpenGL states.
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set up the projection matrix.
    m_ProjectionMatrix = math::matrix4x4_t::Projection2D(
        width, height, 10, -10);

    // Initialize GLFW callback functions.
    glfwSetWindowCloseCallback(util::CEventHandler::WindowCloseCallback);
    glfwSetKeyCallback(util::CEventHandler::KeyboardCallback);
    glfwSetMouseButtonCallback(util::CEventHandler::MouseCallback);
    glfwSetMousePosCallback(util::CEventHandler::MousePosCallback);

    // Initialize GLEW (glewExperimental specifies the latest features).
    glewExperimental = true;
    if(glewInit() != GLEW_OK) return false;

    // This error is caused by glew on core profiles for some reason.
    return (glGetError() == GL_INVALID_ENUM);
}

/**
 * @bug Anything other than black (NULL) blends improperly with scenes.
 **/
void CWindow::Clear(const color4f_t* BGColor)
{
    if(BGColor)
        glClearColor(BGColor->r, BGColor->g, BGColor->b, BGColor->a);
    else
        glClearColor(0, 0, 0, 1);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void CWindow::Update()
{
    glfwSwapBuffers();
}

/**
 * @todo    Implement this.
 **/
bool CWindow::ToggleFullscreen()
{
    return !(m_fullscreen = !m_fullscreen);
}

void CWindow::ToggleVSYNC()
{
    static bool on = true;

    wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)
        wglGetProcAddress("wglSwapIntervalEXT");
    wglSwapIntervalEXT(on = !on);
}
