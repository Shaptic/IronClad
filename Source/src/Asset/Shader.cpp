#include "Asset/Shader.hpp"

using namespace ic;
using asset::CShader;
using util::g_Log;

CShader::~CShader()
{
    this->Release();
}

bool CShader::LoadFromFile(const char* pfilename)
{
    // Infer shader type from filename.
    int type = GL_VERTEX_SHADER;
    if(std::string(pfilename).find(".fs") != std::string::npos)
        type = GL_FRAGMENT_SHADER;

    std::ifstream   file;
    std::string     source, line;
    uint32_t        shader;
    int             length;

    // Load shader source file.
    file.open(pfilename);
    if(!file.is_open())
    {
        util::g_Log.Flush();
        util::g_Log << "[ERROR] File does not exist\n";
        m_error_str = pfilename;
        m_error_str += " does not exist";

        return false;
    }

    while(std::getline(file, line))
    {
        source += line + '\n';
    }

    file.close();
    const char* psrc = source.c_str();

    // Create shader.
    shader = glCreateShader(type);

    // Compile shader.
    length = source.length();
    glShaderSource(shader, 1, &psrc, &length);
    glCompileShader(shader);

    // Check for errors.
    glGetShaderiv(shader, GL_COMPILE_STATUS, &m_error);

    // There is one!
    if(m_error == GL_FALSE)
    {
        char* buf = NULL;

        // Get log length.
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

        // Clear out old log, make a new one and load it.
        m_error_str.clear();
        buf = new char[length];

        glGetShaderInfoLog(shader, length, &length, buf);
        glDeleteShader(shader);

        m_error_str = buf;
        delete[] buf;

        util::g_Log.Flush();
        util::g_Log << "[ERROR] Failed to compile " << pfilename << "\n";
        util::g_Log << "[ERROR] OpenGL error: " << m_error_str << "\n";
        util::g_Log.PrintLastLog();
        return false;
    }

    m_shader    = shader;
    m_filename  = pfilename;
    return true;
}

bool CShader::LoadFromFile(const std::string& filename)
{
    return this->LoadFromFile(filename.c_str());
}

void CShader::Release()
{
    if(m_original)
    {
        g_Log.Flush();
        g_Log << "[DEBUG] Deleted asset: " << m_filename << " (" << m_id << ")\n";
        g_Log.PrintLastLog();
        glDeleteShader(m_shader);
    }
}