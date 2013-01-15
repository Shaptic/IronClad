#include "Graphics/Framebuffer.hpp"

using namespace ic;
using gfx::CFrameBuffer;

CFrameBuffer::~CFrameBuffer()
{
    // Delete everything.
    glDeleteTextures(1, &m_texture);
    glDeleteRenderbuffers(1, &m_db);
    glDeleteFramebuffers(1, &m_fbo);
}

bool CFrameBuffer::Init(const int width, const int height)
{
    // Create texture and allocate memory for full-screen on it.
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
        0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    // Create frame-buffer and attach texture to it.
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
        GL_TEXTURE_2D, m_texture, 0);

    // Create depth buffer and attach to frame-buffer.
    glGenRenderbuffers(1, &m_db);
    glBindRenderbuffer(GL_RENDERBUFFER, m_db);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 
        width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
        GL_RENDERBUFFER, m_db);

    // Check for awesomeness.
    uint32_t status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    // Set up attributes.
    /// @todo Fix view-port setting / resetting.
    glViewport(0, 0, width, height);

    // Unbind everything.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    return (status == GL_FRAMEBUFFER_COMPLETE);
}

void CFrameBuffer::Enable()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

void CFrameBuffer::Disable()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CFrameBuffer::Clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.f, 0.f, 0.f, 1.f);
}