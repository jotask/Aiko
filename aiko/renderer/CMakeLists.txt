cmake_minimum_required(VERSION 3.22)

project(AikoRenderer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

include(Dependencies.cmake)

# It is always easier to navigate in an IDE when projects are organized in folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# generate constant file
set(GLOBAL_PATH ${CMAKE_SOURCE_DIR}/)
set(GLOBAL_ASSET_PATH ${CMAKE_SOURCE_DIR}/assets/)
set(AIKO_RENDER AIKO_BGFX) #AIKO_NATIVE AIKO_BGFX
configure_file(constants.h.in ${CMAKE_CURRENT_SOURCE_DIR}/src/constants.h)

# generate constant file
set(AIKORENDERER_RENDER AIKO_BGFX) #AIKOR_NATIVE AIKO_BGFX

# Files
file(GLOB_RECURSE AIKORENDERER_SRC "src/*.h" "src/*.cpp" "src/*.hpp")

# Organize source files into folders
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${AIKORENDERER_SRC})

# Main Library
add_library(AikoRenderer STATIC ${AIKORENDERER_SRC})
target_compile_definitions(AikoRenderer PRIVATE ${AIKORENDERER_RENDER} )

target_precompile_headers(AikoRenderer PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/src/aiko_pch.h>")

if (WIN32)
    target_compile_definitions(AikoRenderer PUBLIC ${AIKO_CURRENT_PLATFORM})
    message("Compiling for Windows")
elseif (UNIX)
    target_compile_definitions(AikoRenderer PUBLIC ${AIKO_CURRENT_PLATFORM})
    message("Compiling for linux")
else()
    message(FATAL_ERROR "Unsupported OS")
endif()

target_compile_definitions(AikoRenderer PUBLIC NOMINMAX )
 
target_compile_features(AikoRenderer PRIVATE cxx_std_17)
target_link_libraries(AikoRenderer PRIVATE AikoCore glfw glad imgui stb bx bimg bgfx assimp )
target_link_libraries(AikoRenderer PUBLIC magic_enum )

set_target_properties(AikoRenderer PROPERTIES UNITY_BUILD ON)

target_include_directories(AikoRenderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)