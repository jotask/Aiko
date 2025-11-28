#pragma once

#include "models/shader.h"

namespace aiko::texture
{

    class Texture
    {
    public:
        uint id;                  // OpenGL texture id
        int width;                        // Texture base width
        int height;                       // Texture base height
        int mipmaps;                      // Mipmap levels, 1 by default
        int format;                       // Data format (PixelFormat type)
        int channels;
    };

    class PboTexture
    {
    public:
        uint pbo;
        uint data_size;
        Texture texture;
    };

    class RenderTexture2D
    {
    public:
        uint framebuffer;
        Texture texture;
        Texture depth;
    };

    struct ScreenFbo
    {
        uint vao;
        uint vbo;
        texture::RenderTexture2D renderTexture;
    };

}

