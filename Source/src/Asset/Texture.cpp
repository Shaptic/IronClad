#include "IronClad/Asset/Texture.hpp"

using namespace ic;
using asset::CTexture;
using util::g_Log;

CTexture::~CTexture()
{
    this->Release();
}

CTexture& CTexture::operator=(const CTexture& Copy)
{
    m_texture   = Copy.GetTextureID();
    m_height    = Copy.GetH();
    m_width     = Copy.GetW();

    return (*this);
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
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,  &m_width);
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
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,  &m_width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &m_height);
    this->Unbind();

    return true;
}

bool ic::asset::CTexture::LoadFromTexture(const uint32_t texture,
                                          const bool copy)
{
    if(texture == 0) return false;
    
    // The 'copy' flag creates a whole new texture from the given handle.
    // This is useful if you are going to change the data in 'texture,'
    // but don't want the change reflected in the other entities using it.
    if(copy)
    {
        int w, h;
        glBindTexture(GL_TEXTURE_2D, texture);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,  &w);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);

        unsigned char* data = new unsigned char[w * h * 4];
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Since we made a new texture, we need to make sure it gets 
        // cleaned up properly by the asset manager.
        m_original = true;
        m_id = Hash((char*)data, w * h);

        return this->LoadFromRaw(GL_RGBA, GL_RGBA, w, h, data);
    }
    else
    {
        m_texture = texture;

        this->Bind();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,  &m_width);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &m_height);
        this->Unbind();
    }

    return true;
}
