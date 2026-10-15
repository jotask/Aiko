aiko_collect_target_sources(EXAMPLE_PHYSICSLAB_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${EXAMPLE_PHYSICSLAB_SRC})

add_executable(PhysicsLab ${EXAMPLE_PHYSICSLAB_SRC})

target_link_libraries(PhysicsLab PRIVATE Aiko)

target_include_directories(PhysicsLab PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

aiko_apply_defaults(PhysicsLab)

aiko_add_example(PhysicsLab)
