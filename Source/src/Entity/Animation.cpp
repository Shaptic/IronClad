#include "Entity/Animation.hpp"

using namespace ic;
using util::g_Log;

bool CAnimation::LoadFromFile(const std::string& filename,
                              gfx::CVertexBuffer& VBO)
{
    std::ifstream anim(filename, std::ios::binary);

    CAnimation::AnimationHeader header;
    memset(&header, NULL, sizeof header);

    if(!anim.is_open()) return false;

    // Read header
    anim >> header.width;
    anim >> header.height;
    anim >> header.columns;
    anim >> header.rows;

    // Get file size.
    const std::streampos begin = anim.tellg();
    anim.seekg(0, std::ios::end);
    const std::streampos end = anim.tellg();
    anim.seekg(begin);

    // Texture data.
    char* data = new char[end - begin];
    anim.read(data, end - begin);

    GLFWimage img;
    glfwReadMemoryImage(data, end - begin, &img, 0);

    uint16_t w = (header.width);
    uint16_t h = (header.height);

    uint16_t sprite_w = (1.f * w) / (header.columns);
    uint16_t sprite_h = (1.f * h) / (header.rows);

    vertex2_t quad_v[4];
    uint16_t  quad_i[6] = {0, 1, 3, 3, 2, 1};

    quad_v[0].Position = math::vector2_t(0,         0);
    quad_v[1].Position = math::vector2_t(sprite_w,  0);
    quad_v[2].Position = math::vector2_t(sprite_w,  sprite_h);
    quad_v[3].Position = math::vector2_t(0,         sprite_h);

    g_Log.Flush();
    g_Log << "[DEBUG] Loading animation file. ";
    g_Log << (w / sprite_w) * (h / sprite_h) << " expected.\n";
    g_Log.PrintLastLog();

    std::stringstream ss;

    for(size_t i = 0; i < w; i += sprite_w)
    {
        for(size_t j = 0; j < h; j += sprite_h)
        {
            asset::CTexture* pTexture = 
                asset::CAssetManager::Create<asset::CTexture>();
            
            ss << filename << ":" << i * sprite_w << "x" << j * sprite_h;
            pTexture->SetFilename(ss.str());
            ss.str(std::string());

            pTexture->LoadSubFromRaw(img.Format, 
                i * sprite_w, 
                j * sprite_h,
                sprite_w, sprite_h,
                img.Data);

            mp_allTextures.push_back(pTexture);
        }
    }

    glfwFreeImage(&img);

    this->SetMaterialOverride(mp_allTextures[0]);
    //if(mp_allTextures.size() > 0) mp_ActiveTexture = mp_allTextures[0];

    return m_Mesh.LoadMesh(quad_v, 4, quad_i, 6);
}
