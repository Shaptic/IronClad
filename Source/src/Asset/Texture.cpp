#include "Asset/Texture.hpp"

using namespace ic;
using asset::CTexture;
using util::g_Log;

CTexture::~CTexture()
{
    this->Release();
}

bool CTexture::LoadFromFile(const std::string& filename)
{
    return CTexture::LoadFromFile(filename.c_str());
}

bool CTexture::LoadFromFile(const char* pfilename)
{
    if(pfilename == NULL) return false;

    m_texture = util::load_texture(pfilename);
    if(m_texture == 0) return false;

    // Sets up some texture parameters and determines dimensions.
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &m_width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &m_height);
    glBindTexture(GL_TEXTURE_2D, 0);

    m_filename = pfilename;
    return true;
}

void CTexture::Release()
{
    if(m_original)
    {
        g_Log.Flush();
        g_Log << "[DEBUG] Deleted asset: " << m_filename << " (" << m_id << ")\n";
        g_Log.PrintLastLog();

        glDeleteTextures(1, &m_texture);
    }
}
