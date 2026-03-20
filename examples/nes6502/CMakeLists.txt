include(Dependencies.cmake)

set(NES_ASSET_PATH ${CMAKE_CURRENT_SOURCE_DIR}/assets/)
configure_file(constants.h.in ${CMAKE_CURRENT_SOURCE_DIR}/src/constants.h)

file(GLOB_RECURSE EXAMPLE_NESEMULATOR_SRC CONFIGURE_DEPENDS "src/*.h" "src/*.cpp" "src/*.hpp")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${EXAMPLE_NESEMULATOR_SRC})

add_executable(NesEmulator ${EXAMPLE_NESEMULATOR_SRC})

target_link_libraries(NesEmulator PRIVATE Aiko imgui jsoncpp_static)

target_include_directories(NesEmulator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

aiko_apply_defaults(NesEmulator)