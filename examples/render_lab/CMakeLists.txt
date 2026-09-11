aiko_collect_target_sources(EXAMPLE_RENDERLAB_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${EXAMPLE_RENDERLAB_SRC})

add_executable(RenderLab ${EXAMPLE_RENDERLAB_SRC})

target_link_libraries(RenderLab PRIVATE Aiko)

target_include_directories(RenderLab PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

aiko_apply_defaults(RenderLab)
