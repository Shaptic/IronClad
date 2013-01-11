#include "Graphics/VertexBuffer.hpp"

using namespace ic;
using gfx::CVertexBuffer;

CVertexBuffer::CVertexBuffer() : m_vertex_count(0), m_index_count(0)
{
    // Clear everything.
    m_vertexBuffer.clear();
    m_indexBuffer.clear();
    m_enabledAttributes.clear();
    m_enabledAttributes.reserve(3);

    m_vbo       = m_ibo = m_vao = 0;
    m_bo_type   = GL_STATIC_DRAW;

    // Add some default vertex attributes.
    m_enabledAttributes.push_back(0);
    m_enabledAttributes.push_back(1);
    m_enabledAttributes.push_back(2);
}

CVertexBuffer::~CVertexBuffer()
{
    this->Unbind();
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ibo);
}

bool CVertexBuffer::Bind()
{
    if(m_vao == 0 || m_vbo == 0 || m_ibo == 0) return false;

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    
    std::for_each(
        m_enabledAttributes.begin(),
        m_enabledAttributes.end(),
        glEnableVertexAttribArray);

    return (glGetError() == GL_NO_ERROR);
}

bool CVertexBuffer::Unbind()
{
    if(m_vao == 0 || m_vbo == 0 || m_ibo == 0) return false;

    std::for_each(
        m_enabledAttributes.begin(),
        m_enabledAttributes.end(),
        glDisableVertexAttribArray);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);    
    glBindVertexArray(0);
    
    return (glGetError() == GL_NO_ERROR);
}

void CVertexBuffer::Draw()
{
    this->Bind();

    glDrawElements(GL_TRIANGLES, m_index_count,
        GL_UNSIGNED_SHORT, NULL);

    this->Unbind();
}

/**
 * @todo Fix buffers, only works once.
 **/
void CVertexBuffer::AddData(
    vertex2_t*  pVBuffer,   uint32_t vsize,
    uint16_t*   pIBuffer,   uint16_t isize,
    uint32_t*   voffset,    uint32_t* ioffset)
{
    if(vsize == 0 || isize == 0) return;

    // Calculates the offset to access the given buffer at.
    // This is done *before* adding the elements, because
    // the offset points to the beginning of the buffer,
    // rather than the end.
    if(voffset)
        *voffset = m_vertexBuffer.size() * sizeof(pVBuffer[0]);

    // Reserves space for the new buffer and adds it to
    // the internal one. This means that the original is
    // left untouched and can be delete[]'ed whenever.
    m_vertexBuffer.reserve(m_vertexBuffer.size() + vsize);
    for(size_t i = 0; i < vsize; ++i)
        m_vertexBuffer.push_back(pVBuffer[i]);

    // Repeats the process for the index buffer.
    if(ioffset)
        *ioffset = m_indexBuffer.size() * sizeof(pIBuffer[0]);

    m_indexBuffer.reserve(m_indexBuffer.size() + isize);
    for(size_t i = 0; i < isize; ++i)
        m_indexBuffer.push_back(pIBuffer[i]);
}

/**
 * @todo Fix the VAO attribute ID locating.
 **/
void CVertexBuffer::FinalizeBuffer()
{
    // No point in finalizing if it's been done!
    if(this->Finalized()) return;
    
    // Bind the VAO / VBO / IBO for use.
    if(!this->Bind()) return;

    // Check if there's existing data on the buffers.
    int32_t bsize = 0;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bsize);

    // There is!
    if(bsize > 0)
    {
        // Copy existing buffer data from GPU to local buffer.
        vertex2_t* data = (vertex2_t*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
        vertex2_t* vtmp = new vertex2_t[bsize / sizeof(vertex2_t)];
        memcpy(vtmp, data, bsize);
        glUnmapBuffer(GL_ARRAY_BUFFER);

        // Allocate enough GPU space for all vertex data, new and old.
        // Pass the old data directly to it.
        glBufferData(GL_ARRAY_BUFFER,
            bsize + (sizeof m_vertexBuffer[0] * m_vertexBuffer.size()),
            vtmp, m_bo_type);

        // Pass the latest vertex data at the end of the existing data.
        glBufferSubData(GL_ARRAY_BUFFER, 
            (bsize + (sizeof m_vertexBuffer[0] * m_vertexBuffer.size())),
            sizeof m_vertexBuffer[0] * m_vertexBuffer.size(),
            &m_vertexBuffer[0]);
    }
    // No existing buffer or vertices.
    else
    {
        // Allocate enough space for all vertices on GPU.
        glBufferData(GL_ARRAY_BUFFER, 
            sizeof(m_vertexBuffer[0]) * m_vertexBuffer.size(),
            &m_vertexBuffer[0], m_bo_type);
    }

    // Repeat process for index buffer.
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &bsize);
    if(bsize > 0)
    {
        uint16_t* data = (uint16_t*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_ONLY);
        uint16_t* itmp = new uint16_t[bsize / sizeof(uint16_t)];
        memcpy(itmp, data, bsize);
        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

        // Allocate enough GPU space for all vertex data, new and old.
        // Pass the old data directly to it.
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            bsize + (sizeof m_indexBuffer[0] * m_indexBuffer.size()),
            itmp, m_bo_type);

        // Pass the latest vertex data at the end of the existing data.
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 
            (bsize + (sizeof m_indexBuffer[0] * m_indexBuffer.size())),
            sizeof m_indexBuffer[0] * m_indexBuffer.size(),
            &m_indexBuffer[0]);
    }
    else
    {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            sizeof(m_indexBuffer[0]) * m_indexBuffer.size(),
            &m_indexBuffer[0], m_bo_type);
    }

    // Vertices are arranged in memory like so:
    // [ p0, p1, t0, t1, c0, c1, c2, c3 ]
    // (see IronClad/Base/Types.hpp)

    // Specify vertex position arrangement.
    // According to the diagram shown above, the vertex position
    // would start at index 0.
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 
        sizeof(m_vertexBuffer[0]), VBO_OFFSET(0, vertex2_t, Position));

    // Specify texture coordinate position arrangement.
    // According to the diagram, texture coordinates
    // start at index 2.
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
        sizeof(m_vertexBuffer[0]), VBO_OFFSET(0, vertex2_t, TexCoord));

    // Specify the color arrangement.
    // Starting at index 4.
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 
        sizeof(m_vertexBuffer[0]), VBO_OFFSET(0, vertex2_t, Color));

    // We're done, clean up buffers.
    m_vertex_count += m_vertexBuffer.size();
    m_index_count  += m_indexBuffer.size();
    m_vertexBuffer.clear();
    m_indexBuffer.clear();

    this->Unbind();
}

void CVertexBuffer::Clear()
{
    this->Bind();

    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);
    
    m_vertex_count = m_index_count = 0;
    m_vertexBuffer.clear();
    m_indexBuffer.clear();
    
    this->Unbind();
}

bool CVertexBuffer::Init()
{
    // Create the buffers.
    if(!glGenVertexArrays) return false;

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ibo);

    return true;
}
