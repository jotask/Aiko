#pragma once

#include <string>

namespace aiko
{

    class AikoConfig
    {
    public:

        enum RenderType {
            TwoDimensions = 0,
            ThreeDimensions = 1,
        };

        AikoConfig();
        AikoConfig(std::string tittle, uint16_t width, uint64_t height);

        uint16_t width;
        uint16_t height;
        std::string window_tittle;

        RenderType render_type = RenderType::TwoDimensions;

    };

}
