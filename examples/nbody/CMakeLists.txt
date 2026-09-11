aiko_collect_target_sources(EXAMPLE_NBODY_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${EXAMPLE_NBODY_SRC})

add_executable(NBody ${EXAMPLE_NBODY_SRC})

target_link_libraries(NBody PRIVATE Aiko)

target_include_directories(NBody PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

aiko_apply_defaults(NBody)
