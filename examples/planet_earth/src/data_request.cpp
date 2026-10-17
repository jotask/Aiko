#include "data_request.h"

#include <logger/logger.h>

#include <httplib.h>
#include <netcdf.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace pe
{

    bool MapDataRequest::downloadHeightMap(HeightMapResolution resolution)
    {

        const auto spec = getSpec(resolution);
        const std::filesystem::path destination = getHeightMapPath(resolution);

        if (isValidHeightmap(destination, resolution))
        {
            aiko::logger::Log::infoFmt("Using cached heightmap: {}", destination.string());
            return true;
        }

        std::error_code ec;
        std::filesystem::create_directories(destination.parent_path(), ec);

        if (ec)
        {
            aiko::logger::Log::errorFmt("Could not create cache directory: {}", ec.message());
            return false;
        }

        const std::filesystem::path temporary = destination.string() + ".part";

        std::ofstream output(temporary, std::ios::binary | std::ios::trunc);

        if (!output)
        {
            aiko::logger::Log::error("Failed opening cache file");
            return false;
        }

        aiko::logger::Log::infoFmt("Downloading Earth heightmap {}x{}", spec.width, spec.height);

        httplib::Client client(NOAA_HOST);
        client.set_connection_timeout(10);
        client.set_read_timeout(300);
        client.set_follow_location(true);

        // cpp-httplib defaults to a 100 MB payload limit.
        // Allow large datasets, without holding them in RAM.
        client.set_payload_max_length(2ULL * 1024 * 1024 * 1024);

        int httpStatus = 0;
        bool writeFailed = false;

        const auto response = client.Get(
            buildEndpoint(resolution),

            [&](const httplib::Response& headers)
            {
                httpStatus = headers.status;
                return httpStatus == 200;
            },

            [&](const char* data, size_t length)
            {
                output.write(data, static_cast<std::streamsize>(length));

                if (!output)
                {
                    writeFailed = true;
                    return false;
                }

                return true;
            }
        );

        output.close();

        if (!response || !output || response->status != 200)
        {
            if (httpStatus != 0 && httpStatus != 200)
            {
                aiko::logger::Log::errorFmt("NOAA returned HTTP: {}", httpStatus);
            }
            else if (writeFailed || !output)
            {
                aiko::logger::Log::error("Failed writing heightmap to disk");
            }
            else
            {
                aiko::logger::Log::errorFmt("Download failed: {}", httplib::to_string(response.error()));
            }

            std::filesystem::remove(temporary, ec);
            return false;
        }

        if (!isValidHeightmap(temporary, resolution))
        {
            aiko::logger::Log::error("Downloaded heightmap is invalid");
            std::filesystem::remove(temporary, ec);
            return false;
        }

        // On successful validation, replace any invalid cache.
        std::filesystem::rename(temporary, destination, ec);

        if (ec)
        {
            aiko::logger::Log::errorFmt("Failed caching heightmap: {}", ec.message());
            std::filesystem::remove(temporary, ec);
            return false;
        }

        aiko::logger::Log::infoFmt("Heightmap cached at: {}", destination.string());

        return true;

    }

    bool MapDataRequest::isFileCached(HeightMapResolution resolution)
    {
        const std::filesystem::path destination = getHeightMapPath(resolution);
        return std::filesystem::exists(destination);
    }

    void MapDataRequest::clearCache(HeightMapResolution resolution)
    {
        std::error_code ec;
        const std::filesystem::path destination = getHeightMapPath(resolution);
        std::filesystem::remove(destination, ec);

        if (ec)
        {
            aiko::logger::Log::errorFmt("Failed clearing cache at: {}", destination.string());
            return;
        }

        aiko::logger::Log::errorFmt("Cache cleared");

    }

    std::filesystem::path MapDataRequest::getHeightMapPath(HeightMapResolution resolution)
    {

        const auto spec = getSpec(resolution);

        std::filesystem::path cacheRoot;

        if (const char* xdg = std::getenv("XDG_CACHE_HOME"); xdg && *xdg)
        {
            cacheRoot = xdg;
        }
        else if (const char* home = std::getenv("HOME"); home && *home)
        {
            cacheRoot = std::filesystem::path(home) / ".cache";
        }
        else
        {
            cacheRoot = std::filesystem::temp_directory_path();
        }

        const std::string filename =
            "earth_" +
            std::to_string(spec.width) + "x" +
            std::to_string(spec.height) + ".nc";

        return cacheRoot / "aiko" / "planet_earth" / filename;

    }

    EarthHeightmap MapDataRequest::generateHeighMapData(HeightMapResolution resolution)
    {
        const auto filePath = getHeightMapPath(resolution);
        if (isValidHeightmap(filePath, resolution) == false)
        {
            aiko::logger::Log::error("Heightmap not available. Download it first");
            return {};
        }
        const auto spec = getSpec(resolution);

        int fileId = -1;

        const int openResult = nc_open(filePath.string().c_str(), NC_NOWRITE, &fileId);

        if (openResult != NC_NOERR)
        {
            aiko::logger::Log::errorFmt("Failed to open heightmap: {}", nc_strerror(openResult));
            return {};
        }

        int latitudeDim = -1;
        int longitudeDim = -1;

        int latitudeVar = -1;
        int longitudeVar = -1;
        int elevationVar = -1;

        EarthHeightmap result;

        AIKO_ASSERT(nc_inq_dimid(fileId, "latitude", &latitudeDim) == NC_NOERR, "Couldn't get latitute from file");
        AIKO_ASSERT(nc_inq_dimid(fileId, "longitude", &longitudeDim) == NC_NOERR, "Couldn't get latitute from file");

        AIKO_ASSERT(nc_inq_dimlen(fileId, latitudeDim, &result.height) == NC_NOERR, "Couldn't get latitute dimensions");
        AIKO_ASSERT(nc_inq_dimlen(fileId, longitudeDim, &result.width) == NC_NOERR, "Couldn't get longitude dimensions");

        AIKO_ASSERT(nc_inq_varid(fileId, "latitude", &latitudeVar) == NC_NOERR, "Couldn't get latitude");
        AIKO_ASSERT(nc_inq_varid(fileId, "longitude", &longitudeVar) == NC_NOERR, "Couldn't get longitude");
        AIKO_ASSERT(nc_inq_varid(fileId, "z", &elevationVar) == NC_NOERR, "Couldn't get elevation");

        AIKO_ASSERT(result.height == spec.height, "Mismatch current configuration map and loaded file");
        AIKO_ASSERT(result.width == spec.width, "Mismatch current configuration map and loaded file");

        result.latitudes.resize(result.height);
        result.longitudes.resize(result.width);
        result.elevations.resize(result.width * result.height);

        AIKO_ASSERT(nc_get_var_double(fileId, latitudeVar, result.latitudes.data()) == NC_NOERR, "Couldn't read latitudes");
        AIKO_ASSERT(nc_get_var_double(fileId, longitudeVar, result.longitudes.data()) == NC_NOERR, "Couldn't read longitudes");
        AIKO_ASSERT(nc_get_var_float(fileId, elevationVar, result.elevations.data()) == NC_NOERR, "Couldn't read elevations");

        nc_close(fileId);

        return result;

    }

    bool MapDataRequest::isValidHeightmap(std::filesystem::path path, HeightMapResolution resolution) const
    {

        const auto spec = getSpec(resolution);

        int fileId = -1;

        if (nc_open(path.string().c_str(), NC_NOWRITE, &fileId) != NC_NOERR)
        {
            return false;
        }

        int latitudeDim = -1;
        int longitudeDim = -1;
        int elevationVar = -1;

        size_t rows = 0;
        size_t columns = 0;

        bool valid =
            nc_inq_dimid(fileId, "latitude", &latitudeDim) == NC_NOERR &&
            nc_inq_dimid(fileId, "longitude", &longitudeDim) == NC_NOERR &&
            nc_inq_dimlen(fileId, latitudeDim, &rows) == NC_NOERR &&
            nc_inq_dimlen(fileId, longitudeDim, &columns) == NC_NOERR &&
            nc_inq_varid(fileId, "z", &elevationVar) == NC_NOERR &&
            rows == spec.height &&
            columns == spec.width;

        nc_close(fileId);
        return valid;
    }

    MapDataRequest::HeightMapSpec MapDataRequest::getSpec(HeightMapResolution resolution) const
    {
        switch (resolution)
        {
            case HeightMapResolution::R360x180:
            case HeightMapResolution::R720x360:
            case HeightMapResolution::R1440x720:
            case HeightMapResolution::R2160x1080:
            case HeightMapResolution::R4320x2160:
            case HeightMapResolution::R10800x5400:
            case HeightMapResolution::R21600x10800:
                break;

            default:
                throw std::invalid_argument(
                    "Unsupported heightmap resolution"
                );
        }

        const auto stride = static_cast<unsigned>(resolution);

        return {
            stride,
            SOURCE_WIDTH / stride,
            SOURCE_HEIGHT / stride
        };
    }

    std::string MapDataRequest::buildEndpoint(HeightMapResolution resolution) const
    {
        const auto spec = getSpec(resolution);
        const auto stride = std::to_string(spec.stride);

        return
            "/erddap/griddap/ETOPO_2022_v1_60s.nc"
            "?z%5B0%3A" + stride +
            "%3A10799%5D%5B0%3A" + stride +
            "%3A21599%5D";
    }

}
