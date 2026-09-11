aiko_collect_target_sources(EXAMPLE_PERFORMANCE_SMOKE_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${EXAMPLE_PERFORMANCE_SMOKE_SRC})

add_executable(PerformanceSmoke ${EXAMPLE_PERFORMANCE_SMOKE_SRC})

target_link_libraries(PerformanceSmoke PRIVATE Aiko)

target_include_directories(PerformanceSmoke PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

aiko_apply_defaults(PerformanceSmoke)
