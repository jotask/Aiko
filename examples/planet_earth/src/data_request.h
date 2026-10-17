#pragma once

#include "planet_earth_types.h"

#include <string>
#include <filesystem>

namespace pe
{

    // Values represent NOAA's sampling stride.
    enum class HeightMapResolution : unsigned
    {
        R360x180     = 60,          // 0.25 MB
        R720x360     = 30,          // 1 MB
        R1440x720    = 15,          // 4.1 MB
        R2160x1080   = 10,          // 9.3 MB
        R4320x2160   = 5,           // 37 MB
        R10800x5400  = 2,           // 233 MB
        R21600x10800 = 1            // 933 MB
    };

    class MapDataRequest
    {
    public:
        bool downloadHeightMap(HeightMapResolution resolution);
        bool isFileCached(HeightMapResolution resolution);
        void clearCache(HeightMapResolution resolution);
        std::filesystem::path getHeightMapPath(HeightMapResolution resolution);
        EarthHeightmap generateHeighMapData(HeightMapResolution resolution);
    private:
        bool isValidHeightmap(std::filesystem::path, HeightMapResolution resolution) const;

        struct HeightMapSpec
        {
            unsigned stride;
            size_t width;
            size_t height;
        };

        static constexpr const char* NOAA_HOST = "https://oceanwatch.pifsc.noaa.gov";

        static constexpr size_t SOURCE_WIDTH = 21600;
        static constexpr size_t SOURCE_HEIGHT = 10800;

        HeightMapSpec getSpec(HeightMapResolution resolution) const;
        std::string buildEndpoint(HeightMapResolution resolution) const;

    };

}