#pragma once

#include "models/shader.h"

namespace aiko::texture
{

    class Texture
    {
    public:
        unsigned int id;                  // OpenGL texture id
        int width;                        // Texture base width
        int height;                       // Texture base height
        int mipmaps;                      // Mipmap levels, 1 by default
        int format;                       // Data format (PixelFormat type)
        int channels;
    };

    class PboTexture
    {
    public:
        unsigned int pbo;
        unsigned int data_size;
        Texture texture;
    };

    class RenderTexture2D
    {
    public:
        unsigned int framebuffer;
        Texture texture;
        Texture depth;
    };

}

