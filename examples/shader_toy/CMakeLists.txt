file(GLOB_RECURSE EXAMPLE_SHADERTOY_SRC CONFIGURE_DEPENDS "src/*.h" "src/*.cpp" "src/*.hpp")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${EXAMPLE_SHADERTOY_SRC})

add_executable(ShaderToy ${EXAMPLE_SHADERTOY_SRC})

target_link_libraries(ShaderToy PRIVATE Aiko)

target_include_directories(ShaderToy PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

aiko_apply_defaults(ShaderToy)