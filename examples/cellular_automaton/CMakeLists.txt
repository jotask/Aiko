aiko_collect_target_sources(EXAMPLE_CELLULARAUTOMATON_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${EXAMPLE_CELLULARAUTOMATON_SRC})

add_executable(CellularAutomaton ${EXAMPLE_CELLULARAUTOMATON_SRC})

target_link_libraries(CellularAutomaton PRIVATE Aiko tbb)

target_include_directories(CellularAutomaton PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

aiko_apply_defaults(CellularAutomaton)
