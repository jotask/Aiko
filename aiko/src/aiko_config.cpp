#include "aiko_config.h"

namespace aiko
{
    AikoConfig::AikoConfig()
        : window_tittle("Aiko")
        , width(200)
        , height(100)
    {
    }

    AikoConfig::AikoConfig(std::string tittle, uint16_t width, uint64_t height)
        : window_tittle(tittle)
        , width (width)
        , height(height)
    {

    }

}
