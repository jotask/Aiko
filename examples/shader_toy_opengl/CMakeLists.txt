aiko_collect_target_sources(EXAMPLE_NATIVE_SHADERTOY_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${EXAMPLE_NATIVE_SHADERTOY_SRC})

add_executable(NativeShaderToy ${EXAMPLE_NATIVE_SHADERTOY_SRC})

target_link_libraries(NativeShaderToy PRIVATE Aiko)

target_include_directories(NativeShaderToy PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

aiko_apply_defaults(NativeShaderToy)

aiko_add_example(NativeShaderToy)
