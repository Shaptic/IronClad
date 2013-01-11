#include "Utils/Loader.hpp"

using namespace ic;

/*
SDL_Surface* util::load_image(const char* pfilename)
{
    SDL_Surface* pImg = pImg_Load(pfilename);
    if(!pImg) return NULL;

    // Check for 32-bit (RGBA) image.
    if(pImg->format->BytesPerPixel == 4)
    {
        SDL_FreeSurface(pImg);
        return load_image_alpha(pfilename);
    }

    return pImg;
}

SDL_Surface* util::load_image_alpha(const char* pfilename)
{
    SDL_Surface* pImg = IMG_Load(pfilename);
    if(!pImg) return NULL;

    // Check for 24-bit image.
    if(pImg->format->BytesPerPixel == 3)
    {
        SDL_FreeSurface(pImg);
        return util::load_image(pfilename);
    }

    return pImg;
}*/

uint32_t util::load_texture(const char* pfilename)
{
    uint32_t texture = 0;

    // Create a texture and bind it.
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Load texture and set up parameters.
    if(glfwLoadTexture2D(pfilename, 0) == GL_FALSE) return false;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}

/*
uint32_t util::surface_to_texture(SDL_Surface* pSrc)
{
    if(pSrc == NULL) return 0;

    uint32_t texture;
    uint16_t format = pSrc->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, format, pSrc->w, pSrc->h, 0,
        format, GL_UNSIGNED_BYTE, pSrc->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}
*/
