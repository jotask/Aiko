#include "aiko_config.h"

namespace aiko
{

    AikoConfig::AikoConfig()
        : AikoConfig("Aiko", 800, 600)
    {
    }

    AikoConfig::AikoConfig(string title, uint16_t width, uint16_t height)
        : windowTitle(title)
        , width(width)
        , height(height)
    {

    }

}
