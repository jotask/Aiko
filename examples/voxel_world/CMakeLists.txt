include(Dependencies.cmake)

file(GLOB_RECURSE EXAMPLE_VOXEL_WORLD_SRC CONFIGURE_DEPENDS "src/*.h" "src/*.cpp" "src/*.hpp")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${EXAMPLE_VOXEL_WORLD_SRC})

add_executable(VoxelWorld ${EXAMPLE_VOXEL_WORLD_SRC})

target_link_libraries(VoxelWorld PRIVATE Aiko PerlinNoise::PerlinNoise)

target_include_directories(VoxelWorld PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

aiko_apply_defaults(VoxelWorld)