#include "aiko_config.h"

namespace aiko
{
    AikoConfig::AikoConfig()
        : window_tittle("Aiko")
        , width(800)
        , height(600)
        , background_color({ 0.2f, 0.3f, 0.3f, 1.0f })
    {
    }

    AikoConfig::AikoConfig(std::string tittle, uint16_t width, uint64_t height, Color color)
        : window_tittle(tittle)
        , width (width)
        , height(height)
        , background_color(color)
    {

    }

}
