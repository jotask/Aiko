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

    class RenderTexture2D
    {
    public:
        unsigned int framebuffer;       // OpenGL framebuffer object id
        unsigned int texture;           // Color buffer attachment texture
        unsigned int depth;             // Depth buffer attachment texture
        unsigned int width;
        unsigned int height;
    };

}

