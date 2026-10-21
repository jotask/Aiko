aiko_collect_target_sources(AIKO_RESOURCES_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_library(AikoResources STATIC ${AIKO_RESOURCES_SRC})

target_include_directories(AikoResources PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

aiko_set_common_source_layout(AikoResources  "${CMAKE_CURRENT_SOURCE_DIR}/src"  ${AIKO_RESOURCES_SRC})

aiko_apply_defaults(AikoResources)

aiko_add_target(AikoResources)