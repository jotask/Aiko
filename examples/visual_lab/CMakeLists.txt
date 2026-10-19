aiko_collect_target_sources(EXAMPLE_VISUALLAB_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${EXAMPLE_VISUALLAB_SRC})

add_executable(VisualLab ${EXAMPLE_VISUALLAB_SRC})

target_link_libraries(VisualLab PRIVATE Aiko)

target_include_directories(VisualLab PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

aiko_apply_defaults(VisualLab)

aiko_add_example(VisualLab)
