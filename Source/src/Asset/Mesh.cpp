#include "Asset/Mesh.hpp"

using namespace ic;
using asset::CMesh;
using util::g_Log;

CMesh::~CMesh()
{
    this->Release();
}

bool CMesh::LoadFromFile(const char* pfilename)
{
    std::ifstream   file;
    std::string     line;

    file.open(pfilename);
    if(!file.is_open()) return false;

    while(std::getline(file, line))
    {
        // Skip empty lines or comments.
        if(line.empty() || line[0] == '/') continue;

        std::vector<std::string> splitLine = util::split(line, '=');

        // Check for valid key-value line.
        if(splitLine.size() >= 2)
        {
            uint32_t d = 2;

            if(splitLine[0] == "dim")
            {
                d = atoi(splitLine[1].c_str());
            }
            else if(splitLine[0] == "positions")
            {
                std::vector<std::string> vValues = 
                    util::split(splitLine[1], ',');

                m_vBuffer.resize(vValues.size() / 2);

                for(size_t i = 0; i < vValues.size() - 1; i += 2)
                {
                    m_vBuffer[i/2].Position.x = atof(vValues[i].c_str());
                    m_vBuffer[i/2].Position.y = atof(vValues[i+1].c_str());
                }

                vValues.clear();
            }
            else if(splitLine[0] == "texcoords")
            {
                std::vector<std::string> tValues = 
                    util::split(splitLine[1], ',');

                for(size_t i = 0; i < tValues.size() - 1; i += 2)
                {
                    m_vBuffer[i/2].TexCoord.x = atof(tValues[i].c_str());
                    m_vBuffer[i/2].TexCoord.y = atof(tValues[i+1].c_str());
                }

                tValues.clear();
            }
            else if(splitLine[0] == "colors")
            {
                std::vector<std::string> cValues = 
                    util::split(splitLine[1], ',');

                for(size_t i = 0; i < cValues.size() - 3; i += 4)
                {
                    m_vBuffer[i/4].Color.r = atof(cValues[i].c_str());
                    m_vBuffer[i/4].Color.g = atof(cValues[i+1].c_str());
                    m_vBuffer[i/4].Color.b = atof(cValues[i+2].c_str());
                    m_vBuffer[i/4].Color.a = atof(cValues[i+3].c_str());
                }

                cValues.clear();
            }
        }
        else if(splitLine[0] == "[surface]")
        {
            if(!this->LoadSurface(file)) return false;
        }
    }

    // Do merging.
    this->MergeSurfaces();

    // Assign final attributes.
    m_filename  = pfilename;
    m_vcount    = m_vBuffer.size();
    m_icount    = m_iBuffer.size();

    return true;
}

bool CMesh::LoadFromFile(const std::string& filename)
{
    return this->LoadFromFile(filename.c_str());
}

bool CMesh::LoadFromStr(const char** data, const int size)
{
    for(size_t i = 0; i < size; ++i)
    {
        for(size_t j = 0; data[i][j] != '\0'; ++j)
        {
            if(data[i][j] == '/') continue;
            
            std::vector<std::string> splitLine = util::split(data[i], '=');

            // Check for valid key-value line.
            if(splitLine.size() >= 2)
            {
                uint32_t d = 2;

                if(splitLine[0] == "dim")
                {
                    d = atoi(splitLine[1].c_str());
                }
                else if(splitLine[0] == "positions")
                {
                    std::vector<std::string> vValues = 
                        util::split(splitLine[1], ',');

                    m_vBuffer.resize(vValues.size() / 2);

                    for(size_t i = 0; i < vValues.size() - 1; i += 2)
                    {
                        m_vBuffer[i/2].Position.x = atof(vValues[i].c_str());
                        m_vBuffer[i/2].Position.y = atof(vValues[i+1].c_str());
                    }

                    vValues.clear();
                }
                else if(splitLine[0] == "texcoords")
                {
                    std::vector<std::string> tValues = 
                        util::split(splitLine[1], ',');

                    for(size_t i = 0; i < tValues.size() - 1; i += 2)
                    {
                        m_vBuffer[i/2].TexCoord.x = atof(tValues[i].c_str());
                        m_vBuffer[i/2].TexCoord.y = atof(tValues[i+1].c_str());
                    }

                    tValues.clear();
                }
                else if(splitLine[0] == "colors")
                {
                    std::vector<std::string> cValues = 
                        util::split(splitLine[1], ',');

                    for(size_t i = 0; i < cValues.size() - 3; i += 4)
                    {
                        m_vBuffer[i/4].Color.r = atof(cValues[i].c_str());
                        m_vBuffer[i/4].Color.g = atof(cValues[i+1].c_str());
                        m_vBuffer[i/4].Color.b = atof(cValues[i+2].c_str());
                        m_vBuffer[i/4].Color.a = atof(cValues[i+3].c_str());
                    }

                    cValues.clear();
                }
            }
            else if(splitLine[0] == "[surface]")
            {
                if(!this->LoadSurface(data, size)) return false;
            }
        }
    }

    // Do merging.
    this->MergeSurfaces();

    // Assign final attributes.
    m_filename.clear();
    m_vcount    = m_vBuffer.size();
    m_icount    = m_iBuffer.size();

    return true;
}

/**
 * @warning This doesn't work properly, especially with materials.
 * @todo    Create a proper implementation of this.
 **/
bool CMesh::LoadFromRaw(
    const vertex2_t* pvertices, const uint32_t vsize,
    const uint16_t* pindices, const uint32_t isize)
{
    m_vBuffer.clear();
    m_iBuffer.clear();

    for(size_t i = 0; i < vsize; ++i)
        m_vBuffer.push_back(pvertices[i]);

    for(size_t i = 0; i < isize; ++i)
        m_iBuffer.push_back(pindices[i]);
    
    // Create a single surface with isize indices.
    if(!this->LoadSurface(NULL, 0)) return false;
    mp_Surfaces.back()->pMaterial->pTexture = gfx::Globals::g_WhiteTexture;
    mp_Surfaces.back()->icount = isize;

    m_vcount = vsize;
    m_icount = isize;
    return true;
}

bool CMesh::Offload(
    std::vector<vertex2_t>& vbo_buffer,
    std::vector<uint16_t>&  ibo_buffer)
{
    // Verify that a mesh has been loaded.
    if(m_vBuffer.empty() || m_iBuffer.empty()) return false;

    // Calculate the index offset for the various surfaces.
    // This is important to track, as when the mesh is rendered,
    // glDrawElements will use this information to draw the 
    // proper number of vertices.
    // For example, if the surface's local starting point is at the
    // index 8, and the local buffer gets added to the global buffer
    // after index 32, the local buffer's new starting point is index 40.
    uint32_t start = ibo_buffer.size();
    for(size_t i = 0; i < mp_Surfaces.size(); ++i)
        mp_Surfaces[i]->start += start;

    // Load the local indices in the VBO's index buffer.
    ibo_buffer.reserve(ibo_buffer.size() + m_icount);
    for(size_t i = 0; i < m_icount; ++i)
    {
        if(start) m_iBuffer[i] += vbo_buffer.size();
        ibo_buffer.push_back(m_iBuffer[i]);
    }

    // Repeat process for local vertices.
    vbo_buffer.reserve(vbo_buffer.size() + m_vcount);
    for(size_t i = 0; i < m_vcount; ++i)
        vbo_buffer.push_back(m_vBuffer[i]);

    // Delete the local buffers.
    m_iBuffer.clear();
    m_vBuffer.clear();

    return true;
}

void CMesh::MergeSurfaces()
{
    // If there's only one surface in the mesh, there's no need to merge.
    if(mp_Surfaces.size() <= 1) return;

    // Sort surfaces by material.
    std::sort(mp_Surfaces.begin(), mp_Surfaces.end(),
              gfx::surface_t::SortByMaterial);

    // At this point, the surface vector is sorted by material.

    // Now, iterate through the original surfaces and use the start
    // and index count to combine the indices together and align them
    // based on material.
    // This way, draw calls can be batched together much more 
    // efficiently using glDrawElements()
    //
    // So if the indices were originally arranges like so: 
    // |--S1M1--|--S2M2--|--S3M1--|
    // [0, 1, 3, 3, 1, 2, 1, 2, 3]
    // 
    // This would rearrange them to be:
    // |------S1M1------|--S2M2--|
    // [0, 1, 3, 1, 2, 3, 3, 1, 2]
    
    std::vector<uint16_t>           alignedIndices;
    std::vector<gfx::surface_t*>    alignedSurfaces;

    // The current material we're merging.
    gfx::material_t* pActiveMat = mp_Surfaces[0]->pMaterial;

    // The new, merged surface with combined indices and material.
    gfx::surface_t* pAligned    = new gfx::surface_t;

    // Since first, surface starts at index 0 and uses current material.
    pAligned->start     = 0;
    pAligned->icount    = 0;
    pAligned->pMaterial = pActiveMat;

    // Iterate through each surface.
    for(size_t i = 0; i < mp_Surfaces.size(); ++i)
    {
        // In this surface (unchanged), add the old index buffer indices
        // to the aligned one.
        for(size_t j = mp_Surfaces[i]->start; 
            j < mp_Surfaces[i]->start + mp_Surfaces[i]->icount; ++j)
        {
            alignedIndices.push_back(m_iBuffer[j]);
        }

        // If this surface's material matches the one we're merging,
        // increase index count on the merged surface.
        if(mp_Surfaces[i]->pMaterial == pActiveMat)
            pAligned->icount += mp_Surfaces[i]->icount;

        // If it's not, we append the merged surface to the final list,
        // then load the next material to merge on, set the starting
        // index to the last surface's start+index
        // (new_start = old_start + count), and then set the material
        // as the searching one.
        else
        {
            alignedSurfaces.push_back(pAligned);

            pActiveMat          = mp_Surfaces[i]->pMaterial;
            pAligned            = new gfx::surface_t;
            pAligned->start     = alignedSurfaces.back()->start + 
                alignedSurfaces.back()->icount;
            pAligned->icount    = mp_Surfaces[i]->icount;
            pAligned->pMaterial = pActiveMat;
        }
    }

    // Add surface made as loop ended.
    alignedSurfaces.push_back(pAligned);
    
    // Replace the current index buffer with the new aligned indices.
    m_iBuffer.clear();
    m_iBuffer.resize(alignedIndices.size());
    for(size_t i = 0; i < alignedIndices.size(); ++i)
        m_iBuffer[i] = alignedIndices[i];
    alignedIndices.clear();

    // Replace the current surfaces with the new aligned ones.
    for(size_t i = 0; i < mp_Surfaces.size(); ++i)
        delete mp_Surfaces[i];

    mp_Surfaces.clear();
    mp_Surfaces.resize(alignedSurfaces.size());
    for(size_t i = 0; i < alignedSurfaces.size(); ++i)
        mp_Surfaces[i] = alignedSurfaces[i];
    alignedSurfaces.clear();
}

bool CMesh::LoadSurface(const char** lines, const int size)
{
    // Create a new surface.
    gfx::surface_t* pSurface = new gfx::surface_t;
    
    // Load a material into the surface.
    pSurface->pMaterial = new gfx::material_t;

    std::string line;
    std::vector<std::string> splitLine, splitValue;
    
    bool err = false;

    // Calculate this surface's starting offset.
    pSurface->start = 0;
    for(size_t i = 0; i < mp_Surfaces.size(); ++i)
        pSurface->start += mp_Surfaces[i]->icount;

    uint16_t i = 0;
    while(i < size)
    {
        line = std::string(lines[i]);

        // Skip empty lines and comments.
        if(line.empty() || line[0] == '/') continue;

        // If we've reached the next header, move on.
        // But first, if it's another [surface] header, call yourself.
        else if(line == "[surface]")
        {
            mp_Surfaces.push_back(pSurface);
            return this->LoadSurface(lines, size);
        }
        else if(line[0] == '[') break;

        splitLine = util::split(line, '=');
        if(splitLine.size() < 2) continue;

        if(splitLine[0] == "texture")
        {
            // Load a texture asset into the surface.
            pSurface->pMaterial->pTexture = 
                asset::CAssetManager::Create<asset::CTexture>(splitLine[1]);

            err = (pSurface->pMaterial->pTexture == NULL);
        }
        else if(splitLine[0] == "shader")
        {
            // Parse the shader filenames.
            splitValue = util::split(splitLine[1], ',');

            // Load the shader.
            pSurface->pMaterial->pShader = new gfx::CShaderPair;
            err = !pSurface->pMaterial->pShader->LoadFromFile(
                splitValue[0], splitValue[1]);
        }
        else if(splitLine[0] == "indices")
        {
            // Parse indices from the string.
            // The indices are loaded as "1,2,3" inside of the file.
            // See the ICMesh spec for details.
            splitValue = util::split(splitLine[1], ',');
            pSurface->icount = splitValue.size();

            // Reserve space for the new indices (speeds up allocation).
            m_iBuffer.reserve(m_iBuffer.size() + pSurface->icount);

            // Copy the loaded indices for this surface into the buffer.
            for(size_t i = 0; i < splitValue.size(); ++i)
                m_iBuffer.push_back(atoi(splitValue[i].c_str()));
        }
    }

    mp_Surfaces.push_back(pSurface);

    return !err;
}

bool CMesh::LoadSurface(std::ifstream& file)
{
    // Create a new surface.
    gfx::surface_t* pSurface = new gfx::surface_t;

    // Load a material into the surface.
    pSurface->pMaterial = new gfx::material_t;

    std::string line;
    std::vector<std::string> splitLine, splitValue;

    bool err = false;

    // Calculate this surface's starting offset.
    pSurface->start = 0;
    for(size_t i = 0; i < mp_Surfaces.size(); ++i)
        pSurface->start += mp_Surfaces[i]->icount;

    while(std::getline(file, line))
    {
        // Skip empty lines and comments.
        if(line.empty() || line[0] == '/') continue;

        // If we've reached the next header, move on.
        // But first, if it's another [surface] header, call yourself.
        else if(line == "[surface]")
        {
            mp_Surfaces.push_back(pSurface);
            return this->LoadSurface(file);
        }
        else if(line[0] == '[') break;

        splitLine = util::split(line, '=');
        if(splitLine.size() < 2) continue;

        if(splitLine[0] == "texture")
        {
            // Load a texture asset into the surface.
            pSurface->pMaterial->pTexture = 
                asset::CAssetManager::Create<asset::CTexture>(splitLine[1]);

            err = (pSurface->pMaterial->pTexture == NULL);
        }
        else if(splitLine[0] == "shader")
        {
            // Parse the shader filenames.
            splitValue = util::split(splitLine[1], ',');

            // Load the shader.
            pSurface->pMaterial->pShader = new gfx::CShaderPair;
            err = !pSurface->pMaterial->pShader->LoadFromFile(
                splitValue[0], splitValue[1]);
        }
        else if(splitLine[0] == "indices")
        {
            // Parse indices from the string.
            // The indices are loaded as "1,2,3" inside of the file.
            // See the ICMesh spec for details.
            splitValue = util::split(splitLine[1], ',');
            pSurface->icount = splitValue.size();

            // Reserve space for the new indices (speeds up allocation).
            m_iBuffer.reserve(m_iBuffer.size() + pSurface->icount);

            // Copy the loaded indices for this surface into the buffer.
            for(size_t i = 0; i < splitValue.size(); ++i)
                m_iBuffer.push_back(atoi(splitValue[i].c_str()));
        }
    }

    mp_Surfaces.push_back(pSurface);

    return !err;
}

void CMesh::Release()
{
    if(m_original)
    {
        CAsset::Release();
        m_iBuffer.clear();
        m_vBuffer.clear();
    }
}

bool CMesh::LoadFromExisting(std::ifstream& in_file, const std::streampos pos)
{
    std::string line;

    while(in_file.tellg() != pos && std::getline(in_file, line))
    {
        if(line.empty() || line[0] == '/') continue;

        std::vector<std::string> splitLine = util::split(line, '=');

        // Check for valid key-value line.
        if(splitLine.size() >= 2)
        {
            uint32_t d = 2;

            if(splitLine[0] == "dim")
            {
                d = atoi(splitLine[1].c_str());
            }
            else if(splitLine[0] == "positions")
            {
                std::vector<std::string> vValues = 
                    util::split(splitLine[1], ',');

                m_vBuffer.resize(vValues.size() / 2);

                for(size_t i = 0; i < vValues.size() - 1; i += 2)
                {
                    m_vBuffer[i/2].Position.x = atof(vValues[i].c_str());
                    m_vBuffer[i/2].Position.y = atof(vValues[i+1].c_str());
                }

                vValues.clear();
            }
            else if(splitLine[0] == "texcoords")
            {
                std::vector<std::string> tValues = 
                    util::split(splitLine[1], ',');

                for(size_t i = 0; i < tValues.size() - 1; i += 2)
                {
                    m_vBuffer[i/2].TexCoord.x = atof(tValues[i].c_str());
                    m_vBuffer[i/2].TexCoord.y = atof(tValues[i+1].c_str());
                }

                tValues.clear();
            }
            else if(splitLine[0] == "colors")
            {
                std::vector<std::string> cValues = 
                    util::split(splitLine[1], ',');

                for(size_t i = 0; i < cValues.size() - 3; i += 4)
                {
                    m_vBuffer[i/4].Color.r = atof(cValues[i].c_str());
                    m_vBuffer[i/4].Color.g = atof(cValues[i+1].c_str());
                    m_vBuffer[i/4].Color.b = atof(cValues[i+2].c_str());
                    m_vBuffer[i/4].Color.a = atof(cValues[i+3].c_str());
                }

                cValues.clear();
            }
        }
        else if(splitLine[0] == "[surface]")
        {
            if(!this->LoadSurface(in_file)) return false;
        }
    }

    // Do merging.
    this->MergeSurfaces();

    // Assign final attributes.
    m_vcount    = m_vBuffer.size();
    m_icount    = m_iBuffer.size();

    return true;
}

int asset::CMesh::GetMeshWidth() const
{
    int max_value = 0, min_value = 0;

    for(size_t i = 0; i < m_vBuffer.size(); ++i)
    {
        max_value = math::max<int>(max_value, m_vBuffer[i].Position.x);
        min_value = math::min<int>(min_value, m_vBuffer[i].Position.x);
    }

    return (max_value - min_value);
}

int asset::CMesh::GetMeshHeight() const
{
    int max_value = 0, min_value = 0;

    for(size_t i = 0; i < m_vBuffer.size(); ++i)
    {
        max_value = math::max<int>(max_value, m_vBuffer[i].Position.y);
        min_value = math::min<int>(min_value, m_vBuffer[i].Position.y);
    }

    return (max_value - min_value);
}