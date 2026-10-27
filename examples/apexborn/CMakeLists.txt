aiko_collect_target_sources(EXAMPLE_APEXBORN_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${EXAMPLE_APEXBORN_SRC})

add_executable(ApexBorn ${EXAMPLE_APEXBORN_SRC})

target_link_libraries(ApexBorn PRIVATE Aiko Aiko imgui magic_enum)

target_include_directories(ApexBorn PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

aiko_apply_defaults(ApexBorn)

aiko_add_example(ApexBorn)
