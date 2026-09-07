add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/core")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/renderer")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/physics")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/engine")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/editor")

add_library(AikoPublicHeaders INTERFACE)

target_include_directories(AikoPublicHeaders INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/engine/src
        ${CMAKE_CURRENT_SOURCE_DIR}/core/src
        ${CMAKE_CURRENT_SOURCE_DIR}/renderer/src
        ${CMAKE_CURRENT_SOURCE_DIR}/physics/src
)