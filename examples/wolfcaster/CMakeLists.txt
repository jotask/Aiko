
include(Dependencies.cmake)

add_custom_target(WolfcasterData SOURCES ${WOLF3D_DATA_FILES})

aiko_add_dependency(WolfcasterData)

aiko_collect_target_sources(EXAMPLE_WOLFCASTER_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${EXAMPLE_WOLFCASTER_SRC})

add_executable(Wolfcaster ${EXAMPLE_WOLFCASTER_SRC})

target_compile_definitions(Wolfcaster PRIVATE "WOLF3D_DATA_DIR=\"${WOLF3D_SHAREWARE_DATA_DIR}\"")

target_link_libraries(Wolfcaster PRIVATE Aiko imgui magic_enum miniaudio)

target_include_directories(Wolfcaster PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

aiko_apply_defaults(Wolfcaster)

aiko_add_example(Wolfcaster)
