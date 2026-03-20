include(Dependencies.cmake)

# generate constant file
set(GLOBAL_PATH ${CMAKE_SOURCE_DIR}/)
set(GLOBAL_ASSET_PATH ${CMAKE_SOURCE_DIR}/assets/)
configure_file(constants.h.in ${CMAKE_CURRENT_SOURCE_DIR}/src/constants.h)

# Files
file(GLOB_RECURSE AIKORENDERER_SRC "src/*.h" "src/*.cpp" "src/*.hpp")

# Main Library
add_library(AikoRenderer STATIC ${AIKORENDERER_SRC})

aiko_set_common_source_layout(AikoRenderer ${CMAKE_CURRENT_SOURCE_DIR}/src ${AIKORENDERER_SRC})

target_compile_definitions(AikoRenderer PRIVATE ${AIKO_RENDER} )
target_precompile_headers(AikoRenderer PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/src/aiko_pch.h>")

target_link_libraries(AikoRenderer PRIVATE AikoCore glfw glad imgui bx bimg bgfx assimp )
target_link_libraries(AikoRenderer PUBLIC magic_enum )

target_include_directories(AikoRenderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

aiko_apply_defaults(AikoRenderer)