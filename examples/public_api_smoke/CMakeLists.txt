aiko_collect_target_sources(EXAMPLE_RENDER_API_SMOKE_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${EXAMPLE_RENDER_API_SMOKE_SRC})

add_executable(PublicApiSmoke ${EXAMPLE_RENDER_API_SMOKE_SRC})

target_link_libraries(PublicApiSmoke PRIVATE Aiko)

target_include_directories(PublicApiSmoke PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

aiko_apply_defaults(PublicApiSmoke)
