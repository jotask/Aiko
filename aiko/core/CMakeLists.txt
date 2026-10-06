include(Dependencies.cmake)

aiko_collect_target_sources(AIKOCORE_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_library(AikoCore STATIC ${AIKOCORE_SRC})

aiko_set_common_source_layout(AikoCore ${CMAKE_CURRENT_SOURCE_DIR}/src ${AIKOCORE_SRC})

target_link_libraries(AikoCore PRIVATE glm spdlog )
target_link_libraries(AikoCore PUBLIC Tracy::TracyClient )
target_include_directories(AikoCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

aiko_apply_engine_defaults(AikoCore)

aiko_add_target(AikoCore)
