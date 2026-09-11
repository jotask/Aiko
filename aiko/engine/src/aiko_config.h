#pragma once

#include <types/render_config.h>

#include <cstdint>

namespace aiko
{

    class AikoConfig
    {
    public:

        AikoConfig();
        AikoConfig(string title, uint16_t width, uint16_t height);

        RendererConfig renderer;

        uint16_t width;
        uint16_t height;
        string windowTitle;

    };

}
