include(Dependencies.cmake)

# generate constant file
set(GLOBAL_PATH ${CMAKE_SOURCE_DIR}/)
set(GLOBAL_ASSET_PATH ${CMAKE_SOURCE_DIR}/assets/)
configure_file(constants.h.in ${CMAKE_CURRENT_SOURCE_DIR}/src/constants.h)

aiko_collect_target_sources(AIKO_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Main Library
add_library(Aiko STATIC ${AIKO_SRC})

aiko_set_common_source_layout(Aiko ${CMAKE_CURRENT_SOURCE_DIR}/src ${AIKO_SRC})

target_compile_definitions(Aiko PRIVATE ${AIKO_RENDER} )
target_precompile_headers(Aiko PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/src/aiko_pch.h>")
 
target_link_libraries(Aiko PRIVATE assimp stb nlohmann_json::nlohmann_json )
target_link_libraries(Aiko PUBLIC AikoCore AikoRenderer AikoPhysics magic_enum imgui )

target_include_directories(Aiko PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

aiko_apply_defaults(Aiko)
