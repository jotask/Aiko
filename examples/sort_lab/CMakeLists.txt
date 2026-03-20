file(GLOB_RECURSE EXAMPLE_SORTLAB_SRC CONFIGURE_DEPENDS "src/*.h" "src/*.cpp" "src/*.hpp")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${EXAMPLE_SORTLAB_SRC})

add_executable(SortLab ${EXAMPLE_SORTLAB_SRC})

target_link_libraries(SortLab PRIVATE Aiko)

target_include_directories(SortLab PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

aiko_apply_defaults(SortLab)