include(Dependencies.cmake)

aiko_collect_target_sources(EXAMPLE_PLANETEARTH_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${EXAMPLE_PLANETEARTH_SRC})

add_executable(PlanetEarth ${EXAMPLE_PLANETEARTH_SRC})

target_link_libraries(PlanetEarth PRIVATE Aiko httplib::httplib netCDF::netcdf imgui magic_enum)

target_include_directories(PlanetEarth PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

aiko_apply_defaults(PlanetEarth)

aiko_add_example(PlanetEarth)
