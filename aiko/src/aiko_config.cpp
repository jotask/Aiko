#include "aiko_config.h"

namespace aiko
{

    AikoConfig::AikoConfig()
        : AikoConfig("Aiko", 800, 600, DEFAULT_BACKGROUND_COLOR, true)
    {
    }

    AikoConfig::AikoConfig(string tittle, uint16_t width, uint64_t height, Color color, bool auto_render)
        : window_tittle(tittle)
        , width (width)
        , height(height)
        , background_color(color)
        , auto_render(auto_render)
    {

    }

}
