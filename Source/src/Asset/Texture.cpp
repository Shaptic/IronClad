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
    if(pfilename == NULL)
    {
        m_last_error = "No filename given";
        return false;
    }

    m_texture = util::load_texture(pfilename);
    if(m_texture == 0)
    {
        m_last_error = "Texture failed to load";
        return false;
    }

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
        CAsset::Release();
        glDeleteTextures(1, &m_texture);
    }
}

bool CTexture::LoadFromRaw(const int iformat,
                           const int format,
                           const int w, const int h,
                           const unsigned char* buffer )
{
    if(m_texture == 0) glGenTextures(1, &m_texture);

    this->Bind();
    glTexImage2D(GL_TEXTURE_2D, 0, iformat,
                 w, h, 0, format, GL_UNSIGNED_BYTE, buffer);    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);    
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &m_width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &m_height);
    this->Unbind();

    return true;
}
