file(GLOB_RECURSE EXAMPLE_NBODY_SRC CONFIGURE_DEPENDS "src/*.h" "src/*.cpp" "src/*.hpp")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${EXAMPLE_NBODY_SRC})

add_executable(NBody ${EXAMPLE_NBODY_SRC})

target_link_libraries(NBody PRIVATE Aiko)

target_include_directories(NBody PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

aiko_apply_defaults(NBody)