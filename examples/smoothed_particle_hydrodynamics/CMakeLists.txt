aiko_collect_target_sources(EXAMPLE_SPH_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${EXAMPLE_SPH_SRC})

add_executable(SmoothedParticleHydrodynamics ${EXAMPLE_SPH_SRC})

target_link_libraries(SmoothedParticleHydrodynamics PRIVATE Aiko imgui magic_enum)

target_include_directories(SmoothedParticleHydrodynamics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

aiko_apply_defaults(SmoothedParticleHydrodynamics)

aiko_add_example(SmoothedParticleHydrodynamics)
