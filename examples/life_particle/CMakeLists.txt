cmake_minimum_required(VERSION 3.22)

project(ParticleLife LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# It is always easier to navigate in an IDE when projects are organized in folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Files
file(GLOB_RECURSE EXAMPLE_PARTICLE_LIFE_SRC "src/*.h" "src/*.cpp" "src/*.hpp")

# Organize source files into folders
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${EXAMPLE_PARTICLE_LIFE_SRC})

# Main executable
add_executable(ParticleLife ${EXAMPLE_PARTICLE_LIFE_SRC})

target_link_libraries(ParticleLife PRIVATE Aiko)