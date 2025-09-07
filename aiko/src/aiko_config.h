#pragma once

#include <types/color.h>

namespace aiko
{

    class AikoConfig
    {
    public:

        static constexpr Color DEFAULT_BACKGROUND_COLOR = BLACK;

        enum RenderType {
            TwoDimensions = 0,
            ThreeDimensions = 1,
        };

        AikoConfig();
        AikoConfig(string tittle, uint16_t width, uint64_t height, Color = BLACK, bool auto_render = true );

        uint16_t width;
        uint16_t height;
        string window_tittle;
        Color background_color;
        bool auto_render = true;

        RenderType render_type = RenderType::TwoDimensions;

    };

}
