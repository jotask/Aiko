aiko_collect_target_sources(EXAMPLE_VULKAN_SHADERTOY_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${EXAMPLE_VULKAN_SHADERTOY_SRC})

add_executable(VulkanShaderToy ${EXAMPLE_VULKAN_SHADERTOY_SRC})

target_link_libraries(VulkanShaderToy PRIVATE Aiko)

target_include_directories(VulkanShaderToy PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

aiko_apply_defaults(VulkanShaderToy)

aiko_add_example(VulkanShaderToy)
