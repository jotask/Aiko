
include(FetchContent)

# Wolfenstein 3D v1.4 shareware game data.
# Download and extract during CMake configuration.
# This is an asset dependency, not a C++ library.

FetchContent_Declare(
    wolf3d_shareware
    URL https://maniacsvault.net/ecwolf/files/shareware/wolf3d14.zip
    SOURCE_SUBDIR _no_cmake_project
)

FetchContent_MakeAvailable(wolf3d_shareware)

# Locate the extracted map data.
file(GLOB_RECURSE WOLF3D_MAPHEAD_FILES LIST_DIRECTORIES false "${wolf3d_shareware_SOURCE_DIR}/MAPHEAD.WL1")

if(NOT WOLF3D_MAPHEAD_FILES)
    message(FATAL_ERROR "Wolfenstein MAPHEAD.WL1 not found")
endif()

list(GET WOLF3D_MAPHEAD_FILES 0 WOLF3D_MAPHEAD)

get_filename_component(WOLF3D_SHAREWARE_DATA_DIR "${WOLF3D_MAPHEAD}" DIRECTORY)

set(WOLF3D_GAMEMAPS "${WOLF3D_SHAREWARE_DATA_DIR}/GAMEMAPS.WL1")

if(NOT EXISTS "${WOLF3D_GAMEMAPS}")
    message(FATAL_ERROR "Wolfenstein GAMEMAPS.WL1 not found")
endif()

# Verify the expected v1.4 shareware map files.
file(SHA1 "${WOLF3D_MAPHEAD}" WOLF3D_MAPHEAD_HASH)
file(SHA1 "${WOLF3D_GAMEMAPS}" WOLF3D_GAMEMAPS_HASH)

if(NOT WOLF3D_MAPHEAD_HASH STREQUAL "b3942ddb4f6bb671b026a3f85611bf314acefce7")
    message(FATAL_ERROR "Unexpected Wolfenstein MAPHEAD version")
endif()

if(NOT WOLF3D_GAMEMAPS_HASH STREQUAL "d49fc8a7887d7e73be7c813177307b68206f8147")
    message(FATAL_ERROR "Unexpected Wolfenstein GAMEMAPS version")
endif()

message(STATUS "Wolfenstein shareware data: ${WOLF3D_SHAREWARE_DATA_DIR}")
