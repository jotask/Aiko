aiko_collect_target_sources(EXAMPLE_RENDER_REGRESSION_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${EXAMPLE_RENDER_REGRESSION_SRC})

add_executable(RenderRegression ${EXAMPLE_RENDER_REGRESSION_SRC})

target_link_libraries(RenderRegression PRIVATE Aiko)

target_include_directories(RenderRegression PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

aiko_apply_defaults(RenderRegression)
