aiko_collect_target_sources(EXAMPLE_SHADERTOY_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${EXAMPLE_SHADERTOY_SRC})

add_executable(ShaderToy ${EXAMPLE_SHADERTOY_SRC})

target_link_libraries(ShaderToy PRIVATE Aiko)

target_include_directories(ShaderToy PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

aiko_apply_defaults(ShaderToy)
