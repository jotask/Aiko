include(FetchContent)

#----------------------------------------------------------------------

# Enable HTTPS support.
set(HTTPLIB_REQUIRE_OPENSSL ON CACHE BOOL "" FORCE)

# Disable unnecessary features.
set(HTTPLIB_USE_ZLIB_IF_AVAILABLE OFF CACHE BOOL "" FORCE)
set(HTTPLIB_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
        httplib
        GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
        GIT_TAG v0.61.0
        GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(httplib)

#----------------------------------------------------------------------

# Build only the NetCDF functionality required
# for reading classic NetCDF files locally.
set(NETCDF_ENABLE_HDF5 OFF CACHE BOOL "" FORCE)
set(NETCDF_ENABLE_HDF4 OFF CACHE BOOL "" FORCE)
set(NETCDF_ENABLE_NCZARR OFF CACHE BOOL "" FORCE)
set(NETCDF_ENABLE_DAP OFF CACHE BOOL "" FORCE)
set(NETCDF_ENABLE_REMOTE_FUNCTIONALITY OFF CACHE BOOL "" FORCE)

# Disable unnecessary build targets.
set(NETCDF_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
set(NETCDF_ENABLE_EXAMPLES OFF CACHE BOOL "" FORCE)
set(NETCDF_BUILD_UTILITIES OFF CACHE BOOL "" FORCE)

# Disable optional integrations.
set(NETCDF_ENABLE_PLUGINS OFF CACHE BOOL "" FORCE)
set(NETCDF_ENABLE_LIBXML2 OFF CACHE BOOL "" FORCE)
set(NETCDF_ENABLE_FILTER_SZIP OFF CACHE BOOL "" FORCE)
set(NETCDF_ENABLE_FILTER_BZ2 OFF CACHE BOOL "" FORCE)
set(NETCDF_ENABLE_FILTER_BLOSC OFF CACHE BOOL "" FORCE)
set(NETCDF_ENABLE_FILTER_ZSTD OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
        netcdf
        GIT_REPOSITORY https://github.com/Unidata/netcdf-c.git
        GIT_TAG v4.10.1
        GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(netcdf)

#----------------------------------------------------------------------
