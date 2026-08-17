aiko_collect_target_sources(EXAMPLE_SANDBOX_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${EXAMPLE_SANDBOX_SRC})

add_executable(Sandbox ${EXAMPLE_SANDBOX_SRC})

target_link_libraries(Sandbox PRIVATE Aiko)

target_include_directories(Sandbox PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

aiko_apply_defaults(Sandbox)
