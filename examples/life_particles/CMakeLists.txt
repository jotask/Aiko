file(GLOB_RECURSE EXAMPLE_LP_SRC CONFIGURE_DEPENDS "src/*.h" "src/*.cpp" "src/*.hpp")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${EXAMPLE_LP_SRC})

add_executable(LifeParticles ${EXAMPLE_LP_SRC})

target_link_libraries(LifeParticles PRIVATE Aiko)

target_include_directories(LifeParticles PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

aiko_apply_defaults(LifeParticles)