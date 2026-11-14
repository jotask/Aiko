aiko_collect_target_sources(EXAMPLE_SKELETON_LAB_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${EXAMPLE_SKELETON_LAB_SRC})

add_executable(SkeletonLab ${EXAMPLE_SKELETON_LAB_SRC})

target_link_libraries(SkeletonLab PRIVATE Aiko)

target_include_directories(SkeletonLab PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

aiko_apply_defaults(SkeletonLab)

aiko_add_example(SkeletonLab)
