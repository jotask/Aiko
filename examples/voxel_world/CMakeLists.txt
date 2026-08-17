include(Dependencies.cmake)

aiko_collect_target_sources(EXAMPLE_VOXEL_WORLD_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${EXAMPLE_VOXEL_WORLD_SRC})

add_executable(VoxelWorld ${EXAMPLE_VOXEL_WORLD_SRC})

target_link_libraries(VoxelWorld PRIVATE Aiko PerlinNoise::PerlinNoise)

target_include_directories(VoxelWorld PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

aiko_apply_defaults(VoxelWorld)
