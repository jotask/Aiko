cmake_minimum_required(VERSION 3.22)

project(Aiko LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

include(Dependencies.cmake)

# It is always easier to navigate in an IDE when projects are organized in folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Files
file(GLOB_RECURSE AIKO_SRC "src/*.h" "src/*.cpp" "src/*.hpp")

# Organize source files into folders
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${AIKO_SRC})

# Main Library
add_library(Aiko STATIC ${AIKO_SRC})
target_compile_definitions(Aiko PRIVATE AIKO_BUILD_NATIVE)
target_link_libraries(Aiko PRIVATE glfw glad glm TracyClient spdlog imgui EnTT assimp)
target_include_directories(Aiko PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
