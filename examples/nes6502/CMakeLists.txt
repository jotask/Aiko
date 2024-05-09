cmake_minimum_required(VERSION 3.22)

project(NesEmulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# It is always easier to navigate in an IDE when projects are organized in folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Files
file(GLOB_RECURSE EXAMPLE_NESEMULATOR_SRC "src/*.h" "src/*.cpp" "src/*.hpp")

# Organize source files into folders
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${EXAMPLE_NESEMULATOR_SRC})

# Main executable
add_executable(NesEmulator ${EXAMPLE_NESEMULATOR_SRC})

target_link_libraries(NesEmulator PRIVATE Aiko)
target_include_directories(NesEmulator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)