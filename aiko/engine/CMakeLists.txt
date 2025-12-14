cmake_minimum_required(VERSION 3.22)

project(Aiko LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

include(Dependencies.cmake)

# It is always easier to navigate in an IDE when projects are organized in folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# generate constant file
set(GLOBAL_PATH ${CMAKE_SOURCE_DIR}/)
set(GLOBAL_ASSET_PATH ${CMAKE_SOURCE_DIR}/assets/)
set(AIKO_RENDER AIKO_BGFX) #AIKO_NATIVE AIKO_BGFX
configure_file(constants.h.in ${CMAKE_CURRENT_SOURCE_DIR}/src/constants.h)

# Files
file(GLOB_RECURSE AIKO_SRC "src/*.h" "src/*.cpp" "src/*.hpp")

# Organize source files into folders
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${AIKO_SRC})

# Main Library
add_library(Aiko SHARED ${AIKO_SRC})
target_compile_definitions(Aiko PRIVATE ${AIKO_RENDER} )

target_compile_definitions(Aiko PUBLIC NOMINMAX )
 
target_compile_features(Aiko PRIVATE cxx_std_17)
target_link_libraries(Aiko PRIVATE TracyClient EnTT assimp )
target_link_libraries(Aiko PUBLIC AikoCore AikoRenderer magic_enum )

target_include_directories(Aiko PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)