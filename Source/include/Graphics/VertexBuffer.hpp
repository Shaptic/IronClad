/**
 * @file
 *  Graphics/VertexBuffer.hpp - Declarations of the CVertexBuffer 
 *  class, which acts as a wrapper for OpenGL vertex buffer objects.
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
 * @addtogroup Graphics
 * @{
 **/

#ifndef IRON_CLAD__GRAPHICS__VERTEX_BUFFER_HPP
#define IRON_CLAD__GRAPHICS__VERTEX_BUFFER_HPP

#include <vector>
#include <algorithm>

#include "Utils/Utilities.hpp"
#include "Base/Types.hpp"
#include "Window.hpp"

/**
 * Dirty macro to determine the offset of a field within a struct.
 *  Used throughout the CVertexBuffer class to determine the offsets
 *  of data in the GPU buffers.
 **/
#define VBO_OFFSET(count, vertex, field) \
    (void*)((count * sizeof(vertex)) + (long int)&(((vertex*)NULL)->field))

namespace ic
{
namespace gfx
{
    /**
     * A wrapper for OpenGL VBO's.
     *  This wrapper includes many features that allow versatile VBO buffer
     *  management. It's relatively straightforward to add vertices and other
     *  data to the buffer, and once your completely finished, a call to
     *  FinalizeBuffer() will wrap everything up nicely for you, offloading
     *  everything you've specified to the GPU and cleaning up CPU memory.
     **/
    class IRONCLAD_API CVertexBuffer
    {
    public:
        CVertexBuffer();
        ~CVertexBuffer();

        /**
         * Initializes the VBO by creating the necessary buffers.
         *
         * @return  TRUE on success, FALSE on lack of function pointer.
         **/
        bool Init();         

        /**
         * Adds data to the VBO.
         *  The data is only actually put into GPU memory after calling
         *  FinalizeBuffers(), after which nothing else can be added.
         *  Calling this method leaves the passed buffers untouched,
         *  it remains the caller's responsible to clean up after
         *  themselves. This function actually makes a copy of the data,
         *  so any changes made to the original will not be reflected here.
         *
         * @param   vertex2_t*  An array of vertices to pass
         * @param   uint32_t    Vertex array size
         * @param   uint16_t*   An array of indices to pass
         * @param   uint16_t    Index array size
         * @param   uint32_t*   Store vertex offset         (optional=NULL)
         * @param   uint32_t*   Store index offset          (optional=NULL)
         **/
        void AddData(
            vertex2_t*  pVBuffer,   uint32_t vsize,
            uint16_t*   pIBuffer,   uint16_t isize,
            uint32_t*   voffset = NULL,
            uint32_t*   ioffset = NULL);

        /**
         * Offloads everything to the GPU.
         *  Data is stored as [v0|v1|t0|t1|c0|...|v2|v3|...] in the buffer.
         *  All of the copied buffers will be emptied after this call.
         **/
        void FinalizeBuffer();

        /**
         * Binds the VAO, VBO, and IBO for use.
         *  Also, this enables the active vertex shader attributes.
         * 
         * @return  TRUE if no error, FALSE otherwise.
         * @see     AddAttribute(const std::string& attrib_name)
         **/
        bool Bind();

        /**
         * Unbinds the VAO, VBO, and IBO.
         * @return  TRUE if no error, FALSE otherwise.
         **/
        bool Unbind();

        /**
         * Draws the data loaded into the VBO.
         **/
        void Draw();

        /**
         * Specifies the type of VBO (static, dynamic, etc).
         **/
        inline void SetType(const uint16_t type)
        { m_bo_type = type; }

        /**
         * Clears contents of GPU buffers.
         **/
        void Clear();

        /**
         * Determines whether the data has been passed to the GPU yet.
         **/
        inline bool Finalized() const
        { return (m_vertexBuffer.empty() && m_indexBuffer.empty()); }

        /**
         * Provides direct access to the CPU buffer of vertices.
         **/
        inline vertex2_t* GetVertexBuffer()
        { return &m_vertexBuffer[0]; }

        inline std::vector<vertex2_t>& GetVertexBufferVec()
        { return m_vertexBuffer; }

        /**
         * Provides direct access to the CPU index buffer.
         **/
        inline uint16_t* GetIndexBuffer()
        { return &m_indexBuffer[0]; }

        inline std::vector<uint16_t>& GetIndexBufferVec()
        { return m_indexBuffer; }

        /**
         * Easily retrieve the necessary offset for the vertex buffer.
         *  This is useful if, after calling GetVertexBuffer(), the
         *  caller wants to add data to it, but remains unsure as to
         *  what address to put it at.
         *
         * @param   uint32_t&   The offset is loaded into this
         **/
        inline void GetVBOffset(uint32_t& offset)
        { offset = m_vertexBuffer.size() * sizeof(vertex2_t); }

        /**
         * Easily retrieve the necessary offset for the index buffer.
         *  This is useful if, after calling GetIndexBuffer(), the
         *  caller wants to add data to it, but remains unsure as to
         *  what address to put it at.
         *
         * @param   uint32_t&   The offset is loaded into this
         **/
        inline void GetIBOffset(uint32_t& offset)
        { offset = m_indexBuffer.size() * sizeof(uint16_t); }

        /**
         * Provides raw access to the OpenGL VBO.
         **/
        inline uint32_t GetVBO() const
        { return m_vbo; }

        /**
         * Provides raw access to the OpenGL IBO.
         **/
        inline uint32_t GetIBO() const
        { return m_ibo; }

        inline uint32_t GetVCount() const
        { return m_vertex_count; }

        inline uint32_t GetICount() const
        { return m_index_count; }

        inline void* GetTemporaryBuffer(const int buffer_type) const
        { return glMapBuffer(buffer_type, GL_READ_ONLY); }

        inline uint32_t GetError() const 
        { return m_last_error; }

    private:
        std::vector<vertex2_t>  m_vertexBuffer;
        std::vector<uint16_t>   m_indexBuffer;
        std::vector<uint16_t>   m_enabledAttributes;

        uint32_t    m_vbo, m_ibo, m_vao, m_bo_type;
        uint32_t    m_vertex_count, m_index_count;
        uint32_t    m_last_error;
    };

}   // namespace gfx
}   // namespace ic
 
#endif // IRON_CLAD__GRAPHICS__VERTEX_BUFFER_HPP

/** @} **/
