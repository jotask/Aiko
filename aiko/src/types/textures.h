#pragma once

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
    };

    class RenderTexture2D
    {
    public:
        unsigned int id;                  // OpenGL framebuffer object id
        Texture texture;                  // Color buffer attachment texture
        Texture depth;                    // Depth buffer attachment texture
    };

}

