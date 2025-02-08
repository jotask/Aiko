cmake_minimum_required(VERSION 3.22)

project(LoopRecorder LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

include(Dependencies.cmake)

# It is always easier to navigate in an IDE when projects are organized in folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(BUILD_SHARED_LIBS ON)

# Files
file(GLOB_RECURSE EXAMPLE_LR_SRC "src/*.h" "src/*.cpp" "src/*.hpp")

# Organize source files into folders
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${EXAMPLE_LR_SRC})

# Main executable
add_executable(LoopRecorder ${EXAMPLE_LR_SRC})

target_link_libraries(LoopRecorder PRIVATE Aiko imgui portaudio_static)