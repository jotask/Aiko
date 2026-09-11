include(Dependencies.cmake)

aiko_collect_target_sources(EXAMPLE_LR_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${EXAMPLE_LR_SRC})

add_executable(LoopRecorder ${EXAMPLE_LR_SRC})

target_link_libraries(LoopRecorder PRIVATE Aiko imgui portaudio_static)

aiko_apply_defaults(LoopRecorder)
