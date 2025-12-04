cmake_minimum_required(VERSION 3.22)

project(AikoCore LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

include(Dependencies.cmake)

# It is always easier to navigate in an IDE when projects are organized in folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Files
file(GLOB_RECURSE AIKOCORE_SRC "src/*.h" "src/*.cpp" "src/*.hpp")

# Organize source files into folders
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${AIKOCORE_SRC})

# Main Library
add_library(AikoCore STATIC ${AIKOCORE_SRC}
        src/core/transform.h)
target_compile_definitions(AikoCore PRIVATE ${AIKOCORE_CORE} )
target_compile_definitions(AikoCore PRIVATE ${AIKO_CURRENT_PLATFORM} )
target_compile_definitions(AikoCore PUBLIC NOMINMAX )
 
target_compile_features(AikoCore PRIVATE cxx_std_17)
target_link_libraries(AikoCore PRIVATE glm spdlog )

target_include_directories(AikoCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)