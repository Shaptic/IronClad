/**
 * @file
 *  Asset/Mesh.hpp - Defines the CMesh class.
 *
 * @author      George Kudrayvtsev (halcyon)
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
 * @addtogroup Assets
 * @{
 **/

#ifndef IRON_CLAD__ASSETS__MESH_HPP
#define IRON_CLAD__ASSETS__MESH_HPP

#include <vector>
#include <algorithm>

#include "Utils/Utilities.hpp"
#include "Graphics/Globals.hpp"
#include "Graphics/Surface.hpp"
#include "AssetManager.hpp"
#include "Texture.hpp"

namespace ic
{
    // Forward declaration for future friendship.
    namespace gfx { class CMeshInstance; }

namespace asset
{
    /**
     * A helper function that determines whether the given vector
     * contains a given element.
     * 
     * @param   vector<T>&  The vector to find data in
     * @param   T           The element to search for
     * 
     * @return  TRUE if element is in vector, FALSE if not.
     **/
    template<typename T> IRONCLAD_API 
    bool IsIn(const std::vector<T>& data, T finder);

    /**
     * A mesh.
     *  Meshes are simply collections of surfaces, vertex data, and index
     *  data. This data is offloaded into separate buffers (the VBO and IBO
     *  using them) and deleted from RAM completely. Loading may take a 
     *  significant amount of time, based on the size of the mesh and the
     *  amount of surface merging that must take place.
     **/
    class IRONCLAD_API CMesh : public CAsset
    {
    public:
        CMesh(bool orig=false) : CAsset(orig), m_vcount(0), m_icount(0) {}
        ~CMesh();

        /**
         * Loads a mesh from a file.
         *  Mesh files should be structures similar to Wavefront's obj files.
         *  After loading, the surfaces are generated and then merged for
         *  optimization.
         *  The format for mesh files is located in the
         *  "IronClad/Docs/ICMesh.spec" specification file.
         *  
         * @return  TRUE on successful loading, FALSE on error.
         * @see     MergeSurfaces()
         **/
        bool LoadFromFile(const char* pfilename);
        bool LoadFromFile(const std::string& filename);
        bool LoadFromStr(const char** data, const int lines);
        bool LoadFromExisting(std::ifstream& in_file, const std::streampos pos);

        /**
         * Load a mesh directly from vertex and index data.
         *  If this mesh is already loaded, and has *not* been offloaded,
         *  the current mesh data will be deleted and replaced with the
         *  new data given.
         *
         * @param   vertex_t*   Raw vertex data
         * @param   uint32_t    Size of vertex data array
         * @param   uint16_t*   Raw index data
         * @param   uint32_t    Size of index data array
         *
         * @return  TRUE if everything went right, 
         *          FALSE if some problem was encountered.
         **/
        bool LoadFromRaw(const vertex2_t* pvertices, const uint32_t vsize,
                         const uint16_t* pindices,   const uint32_t isize);

        /**
         * Loads current vertex and index data into given buffers.
         *  Each surface of the mesh is subsequently loaded with the 
         *  proper offset data.
         *
         * @param   std::vector<vertex2_t>&     Buffer for vertices to go
         * @param   std::vector<uint16_t>&      Buffer for indices to go
         *
         * @return  TRUE if successfully loaded, FALSE if not, or if
         *          LoadFromFile() has yet to be called.
         *          
         * @post    This mesh now contains no vertex/index data at all.
         **/   
        bool Offload(std::vector<vertex2_t>& vbo_buffer,
                     std::vector<uint16_t>&  ibo_buffer);

        /**
         * Deletes all surface and buffer data.
         **/
        void Clear();

        /**
         * Analyzes the mesh and returns the maximum width/height of it.
         *  If the mesh is a non-quad, it will return the distance from
         *  the right-most point to the left-most point.
         *  If the mesh has been offloaded to the GPU, this will do 
         *  absolutely nothing.
         *
         * @return  Maximum width/height, zero if no vertex data.
         **/
        int GetMeshWidth()  const;
        int GetMeshHeight() const;

        /**
         * Only the CAssetManager and CMeshInstance class can create
         * instances of CMesh assets.
         **/
        friend class CAssetManager;
        friend class gfx::CMeshInstance;

    private:
        void Release();

        /**
         * Performs surface optimization, grouping surfaces with
         * identical materials together.
         **/
        void MergeSurfaces();

        /**
         * Parses a single line from a mesh file.
         * 
         * @param   char*   Line to parse
         * @return  TRUE on successful parse, FALSE otherwise.
         **/
        bool ParseLine(const char* str);

        /**
         * Loads a surface from an .icmesh file.
         * 
         * @param   std::ifstream&  File to read from
         * @return  TRUE if material and surface loaded properly,
         *          FALSE if there was an error, or the file was not 
         *          formatted properly.
         **/
        bool LoadSurface(std::ifstream& file);
        bool LoadSurface(const char** lines, const int size);

        std::vector<gfx::surface_t*>    mp_Surfaces;

        std::vector<vertex2_t>          m_vBuffer;
        std::vector<uint16_t>           m_iBuffer;

        uint32_t    m_vcount, m_icount;
    };

    template<typename T>
    bool IsIn(const std::vector<T>& data, const T& finder)
    {
        for(size_t i = 0; i < data.size(); ++i)
            if(data[i] == finder) return true;

        return false;
    }

}   // namespace asset
}   // namespace ic

#endif // IRON_CLAD__ASSETS__MESH_HPP

/** @} **/
