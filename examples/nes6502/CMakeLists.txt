cmake_minimum_required(VERSION 3.22)

project(Nes6502 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

include(Dependencies.cmake)

# It is always easier to navigate in an IDE when projects are organized in folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Files
file(GLOB_RECURSE EXAMPLE_NES6502_SRC "src/*.h" "src/*.cpp" "src/*.hpp")

# Organize source files into folders
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${EXAMPLE_NES6502_SRC})

# Main executable
add_executable(Nes6502 ${EXAMPLE_NES6502_SRC})

target_link_libraries(Nes6502 PRIVATE Aiko catch)