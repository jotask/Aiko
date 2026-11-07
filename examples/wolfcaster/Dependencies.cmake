
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

# Locate the extracted data directory.
file(GLOB_RECURSE WOLF3D_MAPHEAD_FILES LIST_DIRECTORIES false "${wolf3d_shareware_SOURCE_DIR}/MAPHEAD.WL1")

if(NOT WOLF3D_MAPHEAD_FILES)
    message(FATAL_ERROR "Wolfenstein MAPHEAD.WL1 not found")
endif()

list(GET WOLF3D_MAPHEAD_FILES 0 WOLF3D_MAPHEAD)

get_filename_component(WOLF3D_SHAREWARE_DATA_DIR "${WOLF3D_MAPHEAD}" DIRECTORY)

# Map data.
set(WOLF3D_GAMEMAPS "${WOLF3D_SHAREWARE_DATA_DIR}/GAMEMAPS.WL1")

# Wall textures, sprites and sounds.
set(WOLF3D_VSWAP "${WOLF3D_SHAREWARE_DATA_DIR}/VSWAP.WL1")

# VGA graphics: HUD, menus, fonts and pictures.
set(WOLF3D_VGADICT "${WOLF3D_SHAREWARE_DATA_DIR}/VGADICT.WL1")
set(WOLF3D_VGAGRAPH "${WOLF3D_SHAREWARE_DATA_DIR}/VGAGRAPH.WL1")
set(WOLF3D_VGAHEAD "${WOLF3D_SHAREWARE_DATA_DIR}/VGAHEAD.WL1")

# Audio data.
set(WOLF3D_AUDIOHED "${WOLF3D_SHAREWARE_DATA_DIR}/AUDIOHED.WL1")
set(WOLF3D_AUDIOT "${WOLF3D_SHAREWARE_DATA_DIR}/AUDIOT.WL1")

# All required original game data.
set(WOLF3D_DATA_FILES
        "${WOLF3D_MAPHEAD}"
        "${WOLF3D_GAMEMAPS}"
        "${WOLF3D_VSWAP}"
        "${WOLF3D_VGADICT}"
        "${WOLF3D_VGAGRAPH}"
        "${WOLF3D_VGAHEAD}"
        "${WOLF3D_AUDIOHED}"
        "${WOLF3D_AUDIOT}"
)

# Verify every required file exists.
foreach(WOLF3D_FILE IN LISTS WOLF3D_DATA_FILES)
    if(NOT EXISTS "${WOLF3D_FILE}")
        message(FATAL_ERROR
                "Required Wolfenstein data file not found: ${WOLF3D_FILE}"
        )
    endif()
endforeach()

# Verify the expected v1.4 shareware data.
function(wolf3d_verify_sha1 file_path expected_hash)
    file(SHA1 "${file_path}" actual_hash)

    if(NOT actual_hash STREQUAL expected_hash)
        message(FATAL_ERROR
                "Unexpected Wolfenstein data version: ${file_path}"
        )
    endif()
endfunction()

wolf3d_verify_sha1("${WOLF3D_MAPHEAD}" "b3942ddb4f6bb671b026a3f85611bf314acefce7")
wolf3d_verify_sha1("${WOLF3D_GAMEMAPS}" "d49fc8a7887d7e73be7c813177307b68206f8147")
wolf3d_verify_sha1("${WOLF3D_VSWAP}" "63134a2e3165b5291ece7b8a4145d2bd67d4f787")
wolf3d_verify_sha1("${WOLF3D_VGADICT}" "10ae325f5ce124d2536a63805023c56a4e438ea1")
wolf3d_verify_sha1("${WOLF3D_VGAGRAPH}" "3ad077aca0e775bd6a241515c8b2874d4fa4099a")
wolf3d_verify_sha1("${WOLF3D_VGAHEAD}"  "a9ba5551f54f88d810b9aa82a15caae623509a4c")
wolf3d_verify_sha1("${WOLF3D_AUDIOHED}"  "732460795d77cab19c8018d879f5c27a3cbd5f84")
wolf3d_verify_sha1("${WOLF3D_AUDIOT}"  "0254cc09b7a8ab0b884766b5dcd0e8555c424937")

message(STATUS "Wolfenstein 3D v1.4 shareware data: ${WOLF3D_SHAREWARE_DATA_DIR}")
