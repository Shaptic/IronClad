/**
 * @todo    Make all of the Load* methods use a single thing so that I 
 *          don't have to edit them all every time there's an update.
 **/

#include "IronClad/Asset/Mesh.hpp"

using namespace ic;
using asset::CMesh;
using util::g_Log;

CMesh::~CMesh()
{
    this->Release();
}

CMesh& ic::asset::CMesh::operator=(const CMesh& Copy)
{
    for(size_t i = 0; i < Copy.mp_Surfaces.size(); ++i)
        mp_Surfaces.push_back(Copy.mp_Surfaces[i]);

    m_icount = Copy.m_icount;
    m_vcount = Copy.m_vcount;

    return (*this);
}

bool CMesh::LoadFromFile(const char* pfilename)
{
    std::ifstream   file;
    std::string     line;

    file.open(pfilename);
    if(!file.is_open())
    {
        m_last_error = "File does not exist";
        return false;
    }

    this->Clear();

    util::CParser Parser;

    while(std::getline(file, line))
    {
        // Skip empty lines or comments.
        if(line.empty() || line[0] == '/') continue;

        if(line.find("<entity>") != std::string::npos)
        {
            std::streampos ent_s = file.tellg();
            std::streampos ent_e = util::CParser::FindInFile(file, "</entity>");

            if(ent_e == std::streampos(-1))
            {
                g_Log.Flush();
                g_Log << "[ERROR] Malformed mesh file '" << pfilename;
                g_Log << "': No closing tag found for entity.\n";
                g_Log.PrintLastLog();
                return false;
            }

            // Start from beginning of tag.
            Parser.LoadFromStream(file, ent_s, ent_e, pfilename);
            file.seekg(ent_s);

            // Load preliminary mesh data.

            // Vertices.
            std::vector<std::string> v = Parser.GetValues("vertex", ',');
            if(v.size() <= 1)
            {
                g_Log.Flush();
                g_Log << "[ERROR] Malformed mesh file '" << pfilename;
                g_Log << "': No vertices found for entity.\n";
                g_Log.PrintLastLog();
                return false;
            }

            m_vBuffer.resize(v.size() / 2);
            for(size_t i = 0; i < v.size() - 1; i += 2)
            {
                m_vBuffer[i/2].Position.x = atof(v[i].c_str());
                m_vBuffer[i/2].Position.y = atof(v[i+1].c_str());
            }

            // Texture coordinates.
            v = Parser.GetValues("texcoords", ',');
            if(v.size() <= 1)
            {
                g_Log.Flush();
                g_Log << "[INFO] No texture coordinates found for entity ";
                g_Log << "in mesh: " << pfilename << "\n";
                g_Log.PrintLastLog();
            }
            else
            {
                for(size_t i = 0; i < m_vBuffer.size() << 1; i += 2)
                {
                    m_vBuffer[i/2].TexCoord.x = atof(v[i].c_str());
                    m_vBuffer[i/2].TexCoord.y = atof(v[i+1].c_str());
                }
            }

            // Colors.
            v = Parser.GetValues("colors", ',');
            if(v.size() <= 1)
            {
                g_Log.Flush();
                g_Log << "[INFO] No color specification found for entity ";
                g_Log << "in mesh: " << pfilename << "\n";
                g_Log.PrintLastLog();
            }
            else
            {
                for(size_t i = 0; i < m_vBuffer.size() << 2; i += 4)
                {
                    m_vBuffer[i/4].Color.r = atof(v[i].c_str());
                    m_vBuffer[i/4].Color.g = atof(v[i+1].c_str());
                    m_vBuffer[i/4].Color.b = atof(v[i+2].c_str());
                    m_vBuffer[i/4].Color.a = atof(v[i+3].c_str());
                }
            }

            // Done with preliminary vertex data.
            // Load surfaces now.

            // Search for a surface.
            std::streampos suf_s, suf_e;
            suf_s = util::CParser::FindInFile(file, "<surface>");

            // Load surfaces until no more can be found, or the surface tag
            // is further than the current entity tag.
            while(suf_s != std::streampos(-1) && suf_s < ent_e)
            {
                suf_e = util::CParser::FindInFile(file, "</surface>");

                // Make sure surface end tag isn't further that entity.
                if(suf_e == std::streampos(-1) || suf_e > ent_e) break;

                // Load surface from file stream.
                if(!this->LoadSurface(file, suf_s, suf_e)) break;
            }
        }

        Parser.Reset();
    }

    // Do merging.
    this->MergeSurfaces();

    file.close();

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

/*
bool CMesh::LoadFromStr(const char** data, const int size)
{
    this->Clear();

    util::CParser Parser;

    for(size_t i = 0; i < size; ++i)
    {
        std::string line(data[i]);

        // Skip empty lines or comments.
        if(line.empty() || line[0] == '/') continue;

        if(line.find("<entity>") != std::string::npos)
        {
            std::streampos ent_s = file.tellg();
            std::streampos ent_e = util::CParser::FindInFile(file, "</entity>");

            if(ent_e == std::streampos(-1))
            {
                g_Log.Flush();
                g_Log << "[ERROR] Malformed raw mesh file: ";
                g_Log << "No closing tag found for entity.\n";
                g_Log.PrintLastLog();
                return false;
            }

            // Start from beginning of tag.
            Parser.LoadFromStream(file, ent_s, ent_e);
            file.seekg(ent_s);

            // Load preliminary mesh data.

            // Vertices.
            std::vector<std::string> v = Parser.GetValues("vertex", ',');
            if(v.size() <= 1)
            {
                g_Log.Flush();
                g_Log << "[ERROR] Malformed raw mesh file: ";
                g_Log << "No vertices found for entity.\n";
                g_Log.PrintLastLog();
                return false;
            }

            m_vBuffer.resize(v.size() / 2);
            for(size_t i = 0; i < v.size() - 1; i += 2)
            {
                m_vBuffer[i/2].Position.x = atof(v[i].c_str());
                m_vBuffer[i/2].Position.y = atof(v[i+1].c_str());
            }

            // Texture coordinates.
            v = Parser.GetValues("texcoords", ',');
            if(v.size() <= 1)
            {
                g_Log.Flush();
                g_Log << "[INFO] No texture coordinates found for entity ";
                g_Log << "in raw mesh.\n"
                g_Log.PrintLastLog();
            }
            else
            {
                for(size_t i = 0; i < m_vBuffer.size() << 1; i += 2)
                {
                    m_vBuffer[i/2].TexCoord.x = atof(v[i].c_str());
                    m_vBuffer[i/2].TexCoord.y = atof(v[i+1].c_str());
                }
            }

            // Colors.
            v = Parser.GetValues("colors", ',');
            if(v.size() <= 1)
            {
                g_Log.Flush();
                g_Log << "[INFO] No color specification found for entity ";
                g_Log << "in raw mesh.\n";
                g_Log.PrintLastLog();
            }
            else
            {
                for(size_t i = 0; i < m_vBuffer.size() << 2; i += 4)
                {
                    m_vBuffer[i/4].Color.r = atof(v[i].c_str());
                    m_vBuffer[i/4].Color.g = atof(v[i+1].c_str());
                    m_vBuffer[i/4].Color.b = atof(v[i+2].c_str());
                    m_vBuffer[i/4].Color.a = atof(v[i+3].c_str());
                }
            }

            // Done with preliminary vertex data.
            // Load surfaces now.

            // Search for a surface.
            std::streampos suf_s, suf_e;
            suf_s = util::CParser::FindInFile(file, "<surface>");

            // Load surfaces until no more can be found, or the surface tag
            // is further than the current entity tag.
            while(suf_s != std::streampos(-1) && suf_s < ent_e)
            {
                suf_e = util::CParser::FindInFile(file, "</surface>");

                // Make sure surface end tag isn't further that entity.
                if(suf_e == std::streampos(-1) || suf_e > ent_e) break;

                // Load surface from file stream.
                if(!this->LoadSurface(file, suf_s, suf_e)) break;
            }
        }

        Parser.Reset();
    }

    // Do merging.
    this->MergeSurfaces();

    // Assign final attributes.
    m_filename.clear();
    m_vcount    = m_vBuffer.size();
    m_icount    = m_iBuffer.size();

    return true;
}
*/

bool CMesh::LoadFromExisting(std::ifstream& file, 
                             const std::streampos& end)
{
    if(!file.is_open())
    {
        m_last_error = "File does not exist";
        return false;
    }

    this->Clear();

    std::string line;
    util::CParser Parser;

    while(std::getline(file, line) && file.tellg() < end)
    {
        // Skip empty lines or comments.
        if(line.empty() || line[0] == '/') continue;

        if(line.find("<entity>") != std::string::npos)
        {
            std::streampos ent_s = file.tellg();
            std::streampos ent_e = util::CParser::FindInFile(file, "</entity>");

            if(ent_e == std::streampos(-1))
            {
                g_Log.Flush();
                g_Log << "[ERROR] Malformed mesh stream: ";
                g_Log << "No closing tag found for entity.\n";
                g_Log.PrintLastLog();
                return false;
            }

            // Start from beginning of tag.
            Parser.LoadFromStream(file, ent_s, ent_e);
            file.seekg(ent_s);

            // Load preliminary mesh data.

            // Vertices.
            std::vector<std::string> v = Parser.GetValues("vertex", ',');
            if(v.size() <= 1)
            {
                g_Log.Flush();
                g_Log << "[ERROR] Malformed mesh stream: ";
                g_Log << "No vertices found for entity.\n";
                g_Log.PrintLastLog();
                return false;
            }

            m_vBuffer.resize(v.size() / 2);
            for(size_t i = 0; i < v.size() - 1; i += 2)
            {
                m_vBuffer[i/2].Position.x = atof(v[i].c_str());
                m_vBuffer[i/2].Position.y = atof(v[i+1].c_str());
            }

            // Texture coordinates.
            v = Parser.GetValues("texcoords", ',');
            if(v.size() <= 1)
            {
                g_Log.Flush();
                g_Log << "[INFO] No texture coordinates found for entity ";
                g_Log << "in mesh stream.\n";
                g_Log.PrintLastLog();
            }
            else
            {
                for(size_t i = 0; i < m_vBuffer.size() << 1; i += 2)
                {
                    m_vBuffer[i/2].TexCoord.x = atof(v[i].c_str());
                    m_vBuffer[i/2].TexCoord.y = atof(v[i+1].c_str());
                }
            }

            // Colors.
            v = Parser.GetValues("colors", ',');
            if(v.size() <= 1)
            {
                g_Log.Flush();
                g_Log << "[INFO] No color specification found for entity ";
                g_Log << "in mesh stream.\n";
                g_Log.PrintLastLog();
            }
            else
            {
                for(size_t i = 0; i < m_vBuffer.size() << 2; i += 4)
                {
                    m_vBuffer[i/4].Color.r = atof(v[i].c_str());
                    m_vBuffer[i/4].Color.g = atof(v[i+1].c_str());
                    m_vBuffer[i/4].Color.b = atof(v[i+2].c_str());
                    m_vBuffer[i/4].Color.a = atof(v[i+3].c_str());
                }
            }

            // Done with preliminary vertex data.
            // Load surfaces now.

            // Search for a surface.
            std::streampos suf_s, suf_e;
            suf_s = util::CParser::FindInFile(file, "<surface>", ent_e);

            // Load surfaces until no more can be found, or the surface tag
            // is further than the current entity tag.
            while(suf_s != std::streampos(-1) && suf_s < ent_e)
            {
                suf_e = util::CParser::FindInFile(file, "</surface>", ent_e);

                // Make sure surface end tag isn't further that entity.
                if(suf_e == std::streampos(-1) || suf_e > ent_e) break;

                // Load surface from file stream.
                if(!this->LoadSurface(file, suf_s, suf_e)) break;

                suf_s = util::CParser::FindInFile(file, "<surface>", ent_e);
            }
        }

        Parser.Reset();
    }

    // Do merging.
    this->MergeSurfaces();

    // Assign final attributes.
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
    const uint16_t*  pindices,  const uint32_t isize)
{
    this->Clear();

    m_vBuffer.reserve(vsize);
    m_iBuffer.reserve(isize);
    
    for(size_t i = 0; i < vsize; ++i) m_vBuffer.push_back(pvertices[i]);
    for(size_t i = 0; i < isize; ++i) m_iBuffer.push_back(pindices[i]);
    
    // Create a single surface with isize indices.
    if(!this->LoadSurface(NULL, 0, 0)) return false;
    mp_Surfaces.back()->pMaterial->pTexture = gfx::Globals::g_WhiteTexture;
    mp_Surfaces.back()->icount = isize;

    m_vcount = vsize;
    m_icount = isize;
    return true;
}

bool CMesh::Offload(gfx::CVertexBuffer& VBO)
{
    std::vector<uint16_t>&  ibo_buffer = VBO.GetIndexBufferVec();
    std::vector<vertex2_t>& vbo_buffer = VBO.GetVertexBufferVec();

    // Verify that a mesh has been loaded.
    if(m_vBuffer.empty() || m_iBuffer.empty()) return false;

    // Calculate the index offset for the various surfaces.
    // This is important to track, as when the mesh is rendered,
    // glDrawElements will use this information to draw the 
    // proper number of vertices.
    // 
    // For example, if the surface's local starting point is at the
    // index 8, and the local buffer gets added to the global buffer
    // after index 32, the local buffer's new starting point is index 40.
    uint32_t start = ibo_buffer.size() + VBO.GetICount();
    for(size_t i = 0; i < mp_Surfaces.size(); ++i)
        mp_Surfaces[i]->start += start;

    // Load the local indices in the VBO's index buffer.
    ibo_buffer.reserve(ibo_buffer.size() + m_icount);
    for(size_t i = 0; i < m_icount; ++i)
    {
        if(start) m_iBuffer[i] += vbo_buffer.size() + VBO.GetVCount();
        ibo_buffer.push_back(m_iBuffer[i]);
    }

    // Repeat process for local vertices.
    vbo_buffer.reserve(vbo_buffer.size() + m_vcount);
    for(size_t i = 0; i < m_vcount; ++i)
        vbo_buffer.push_back(m_vBuffer[i]);

    // Delete the local buffers.
    m_iBuffer.clear();
    m_vBuffer.clear();

    this->SetOwner(&VBO);
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

bool CMesh::LoadSurface(const char** lines, const uint32_t s,
                        const uint32_t e)
{
    // Create a new surface.
    gfx::surface_t* pSurface = new gfx::surface_t;

    // Load a material into the surface.
    pSurface->pMaterial = new gfx::material_t;

    // We are optimists now.
    bool success = true;

    // Calculate this surface's starting offset.
    pSurface->start = 0;
    for(size_t i = 0; i < mp_Surfaces.size(); ++i)
        pSurface->start += mp_Surfaces[i]->icount;

    util::CParser Parser;
    Parser.LoadFromStream(lines, s, e);

    std::string data, data2;

    // Load texture into mesh.
    data = Parser.GetValue("texture");
    if(!data.empty())
    {
        // Load a texture asset into the surface.
        pSurface->pMaterial->pTexture = 
            asset::CAssetManager::Create<asset::CTexture>(data);

        success = (pSurface->pMaterial->pTexture != NULL);
    }

    // Load shaders into mesh.
    data  = Parser.GetValue("vshader");
    data2 = Parser.GetValue("fshader");

    // No shader?
    if(data.empty() && data2.empty());

    // Both shaders?
    else if(!(data.empty() && data2.empty()))
    {
        // Load the shader.
        pSurface->pMaterial->pShader = new gfx::CShaderPair;
        success = pSurface->pMaterial->pShader->LoadFromFile(data, data2);
    }

    // One shader?
    else
    {
        if(data.empty())    data = "Shaders/Default.vs";
        if(data2.empty())   data2 = "Shaders/Default.fs";

        // Load the shader.
        pSurface->pMaterial->pShader = new gfx::CShaderPair;
        success = pSurface->pMaterial->pShader->LoadFromFile(data, data2);
    }

    std::vector<std::string> ind = Parser.GetValues("indices", ',');

    if(ind.size() == 1 && ind[0].empty())
    {
        mp_Surfaces.push_back(pSurface);
        return success;
    }

    pSurface->icount = ind.size();

    // Reserve space for the new indices (speeds up allocation).
    m_iBuffer.reserve(m_iBuffer.size() + pSurface->icount);

    // Copy the loaded indices for this surface into the buffer.
    for(size_t i = 0; i < ind.size(); ++i)
        m_iBuffer.push_back(atoi(ind[i].c_str()));

    mp_Surfaces.push_back(pSurface);
    return success;
}

bool CMesh::LoadSurface(std::ifstream& file,
                        const std::streampos& s, 
                        const std::streampos& e)
{
    // Create a new surface.
    gfx::surface_t* pSurface = new gfx::surface_t;

    // Load a material into the surface.
    pSurface->pMaterial = new gfx::material_t;

    // We are pessimists.
    bool success = false;

    // Calculate this surface's starting offset.
    pSurface->start = 0;
    for(size_t i = 0; i < mp_Surfaces.size(); ++i)
        pSurface->start += mp_Surfaces[i]->icount;

    util::CParser Parser;
    Parser.LoadFromStream(file, s, e);

    std::string data, data2;

    // Load texture into mesh.
    data = Parser.GetValue("texture");
    if(!data.empty())
    {
        // Load a texture asset into the surface.
        pSurface->pMaterial->pTexture = 
            asset::CAssetManager::Create<asset::CTexture>(data);

        success = (pSurface->pMaterial->pTexture != NULL);
    }

    // Load shaders into mesh.
    data = Parser.GetValue("vshader");
    data2 = Parser.GetValue("fshader");

    // No shader?
    if(data.empty() && data2.empty());

    // Both shaders?
    else if(!(data.empty() && data2.empty()))
    {
        // Load the shader.
        pSurface->pMaterial->pShader = new gfx::CShaderPair;
        success = pSurface->pMaterial->pShader->LoadFromFile(data, data2);
    }

    // One shader?
    else
    {
        if(data.empty())    data = "Shaders/Default.vs";
        if(data2.empty())   data2 = "Shaders/Default.fs";

        // Load the shader.
        pSurface->pMaterial->pShader = new gfx::CShaderPair;
        success = pSurface->pMaterial->pShader->LoadFromFile(data, data2);
    }

    std::vector<std::string> ind = Parser.GetValues("indices", ',');
    
    pSurface->icount = ind.size();

    // Reserve space for the new indices (speeds up allocation).
    m_iBuffer.reserve(m_iBuffer.size() + pSurface->icount);

    // Copy the loaded indices for this surface into the buffer.
    for(size_t i = 0; i < ind.size(); ++i)
        m_iBuffer.push_back(atoi(ind[i].c_str()));

    file.seekg(e);
    mp_Surfaces.push_back(pSurface);
    return success;
}

void CMesh::Release()
{
    if(m_original)
    {
        CAsset::Release();
        this->Clear();
    }
}

int CMesh::GetMeshWidth() const
{
    int max_value = 0, min_value = 0;

    for(size_t i = 0; i < m_vBuffer.size(); ++i)
    {
        max_value = math::max<int>(max_value, m_vBuffer[i].Position.x);
        min_value = math::min<int>(min_value, m_vBuffer[i].Position.x);
    }

    return (max_value - min_value);
}

int CMesh::GetMeshHeight() const
{
    int max_value = 0, min_value = 0;

    for(size_t i = 0; i < m_vBuffer.size(); ++i)
    {
        max_value = math::max<int>(max_value, m_vBuffer[i].Position.y);
        min_value = math::min<int>(min_value, m_vBuffer[i].Position.y);
    }

    return (max_value - min_value);
}

void CMesh::Clear()
{
    for(size_t i = 0; i < mp_Surfaces.size(); ++i) delete mp_Surfaces[i];

    mp_Surfaces.clear();
    m_vBuffer.clear();
    m_iBuffer.clear();
}
