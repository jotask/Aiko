include(Dependencies.cmake)

aiko_collect_target_sources(AIKOPHYSICS_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Main Library
add_library(AikoPhysics STATIC ${AIKOPHYSICS_SRC})

aiko_set_common_source_layout(AikoPhysics ${CMAKE_CURRENT_SOURCE_DIR}/src ${AIKOPHYSICS_SRC})
 
target_link_libraries(AikoPhysics PRIVATE Jolt::Jolt AikoCore )

target_include_directories(AikoPhysics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

aiko_apply_defaults(AikoPhysics)
