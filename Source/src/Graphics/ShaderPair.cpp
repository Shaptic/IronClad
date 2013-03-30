#include "IronClad/Graphics/ShaderPair.hpp"

using namespace ic;
using gfx::CShaderPair;

CShaderPair::CShaderPair() : m_program(0), mp_VShader(NULL),
    mp_FShader(NULL), m_error_str("No error"), m_error(GL_NO_ERROR) {}

CShaderPair::~CShaderPair()
{
    glDeleteProgram(m_program);
}

short CShaderPair::GetAttributeLocation(const char* attr) const
{
    return glGetAttribLocation(m_program, attr);
}

short CShaderPair::GetUniformLocation(const char* uni) const
{
    return glGetUniformLocation(m_program, uni);
}

bool CShaderPair::LoadFromFile(const char* pvs_filename,
    const char* pfs_filename)
{
    // Load shaders and validate them.
    mp_VShader = asset::CAssetManager::Create<asset::CShader>(pvs_filename);
    mp_FShader = asset::CAssetManager::Create<asset::CShader>(pfs_filename);

    if(!mp_VShader || !mp_FShader) return false;

    util::g_Log.Flush();
    util::g_Log << "[INFO] Linking shader objects.\n";

    // Create shader program and attach shaders.
    m_program = glCreateProgram();
    glAttachShader(m_program, mp_VShader->GetShaderObject());
    glAttachShader(m_program, mp_FShader->GetShaderObject());

    // Link the compiled shader objects to the program.
    glLinkProgram(m_program);
    glGetProgramiv(m_program, GL_LINK_STATUS, &m_error);

    // Link failed?
    if(m_error == GL_FALSE)
    {
        int length  = 0;
        char* buf   = NULL;

        // Get log length to make an appropriate buffer.
        glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &length);

        // Delete old log.
        m_error_str.clear();

        // Get log.
        buf = new char[length];
        glGetProgramInfoLog(m_program, length, &length, buf);
        glDeleteProgram(m_program);
        m_error_str = buf;
        delete[] buf;

        // Show log.
        util::g_Log.Flush();
        util::g_Log << "[ERROR] Failed to link shader objects.\n";
        util::g_Log << "[ERROR] OpenGL error: " << m_error_str << "\n";
        util::g_Log.PrintLastLog();

        return false;
    }

    return true;
}

bool CShaderPair::LoadFromFile(const std::string& vs_filename,
    const std::string& fs_filename)
{
    return this->LoadFromFile(vs_filename.c_str(), fs_filename.c_str());
}

void CShaderPair::Bind()
{
    glUseProgram(m_program);
}

void CShaderPair::Unbind()
{
    glUseProgram(0);
}

const std::string& CShaderPair::GetError() const
{
    if(m_error_str == "No error")
    {
        if(mp_VShader && mp_VShader->GetError() != "No error")
            return mp_VShader->GetError();
        else if(mp_FShader && mp_FShader->GetError() != "No error")
            return mp_FShader->GetError();
    }

    return m_error_str;
}

CShaderPair& CShaderPair::operator=(const CShaderPair& Copy)
{
    m_program = Copy.GetProgram();
    return (*this);
}
