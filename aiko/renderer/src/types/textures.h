#pragma once

#include <aiko_types.h>

namespace aiko::texture
{

    class Texture
    {
    public:
        uint id;                  // Texture id
        int width;                // Texture base width
        int height;               // Texture base height
        int mipmaps;              // Mipmap levels, 1 by default
        int format;               // Data format (PixelFormat type)
        int channels;
    };

}

