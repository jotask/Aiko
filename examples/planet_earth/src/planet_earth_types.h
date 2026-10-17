#pragma once

#include <vector>

namespace pe
{

    struct EarthHeightmap
    {
        size_t width = 0;
        size_t height = 0;

        std::vector<double> latitudes;
        std::vector<double> longitudes;
        std::vector<float> elevations;
    };

}

