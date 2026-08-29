include(Dependencies.cmake)

# It is always easier to navigate in an IDE when projects are organized in folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

aiko_collect_target_sources(AIKOCORE_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_library(AikoCore STATIC ${AIKOCORE_SRC})

aiko_set_common_source_layout(AikoCore ${CMAKE_CURRENT_SOURCE_DIR}/src ${AIKOCORE_SRC})

target_link_libraries(AikoCore PRIVATE glm spdlog )
target_link_libraries(AikoCore PUBLIC Tracy::TracyClient )
target_include_directories(AikoCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

aiko_apply_defaults(AikoCore)
