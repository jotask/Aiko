include(Dependencies.cmake)

file(GLOB_RECURSE EXAMPLE_LR_SRC CONFIGURE_DEPENDS "src/*.h" "src/*.cpp" "src/*.hpp")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${EXAMPLE_LR_SRC})

add_executable(LoopRecorder ${EXAMPLE_LR_SRC})

target_link_libraries(LoopRecorder PRIVATE Aiko imgui portaudio_static)

aiko_apply_defaults(LoopRecorder)