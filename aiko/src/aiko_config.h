#pragma once

#include <string>

class AikoConfig
{
public:

    enum RenderType {
        TwoDimensions = 0,
        ThreeDimensions = 1,
    };

    int Width = 200;
    int Height = 100;
    std::string window_tittle = "Aiko";
    RenderType render_type = RenderType::TwoDimensions;

};

