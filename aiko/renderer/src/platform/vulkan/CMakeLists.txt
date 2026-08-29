set(VULKAN_HEADERS_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
set(VULKAN_HEADERS_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
set(VULKAN_HEADERS_ENABLE_MODULE OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
        VulkanHeaders
        GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Headers.git
        GIT_TAG main
        GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(VulkanHeaders)

set(VOLK_PULL_IN_VULKAN ON CACHE BOOL "" FORCE)
set(VOLK_INSTALL OFF CACHE BOOL "" FORCE)
set(VOLK_HEADERS_ONLY OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
        volk
        GIT_REPOSITORY https://github.com/zeux/volk.git
        GIT_TAG master
        GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(volk)

set(SPIRV_REFLECT_EXECUTABLE OFF CACHE BOOL "" FORCE)
set(SPIRV_REFLECT_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SPIRV_REFLECT_STATIC_LIB ON CACHE BOOL "" FORCE)
set(SPIRV_REFLECT_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
        spirv_reflect
        GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Reflect.git
        GIT_TAG main
        GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(spirv_reflect)

aiko_collect_target_sources(AIKORENDERER_VULKAN_SRC "${CMAKE_CURRENT_SOURCE_DIR}")

target_sources(AikoRenderer PRIVATE ${AIKORENDERER_VULKAN_SRC})
target_sources(imgui PRIVATE ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp)

target_compile_definitions(AikoRenderer PRIVATE VK_NO_PROTOTYPES GLFW_INCLUDE_NONE)
target_compile_definitions(imgui PUBLIC IMGUI_IMPL_VULKAN_USE_VOLK VK_NO_PROTOTYPES GLFW_INCLUDE_NONE)

target_link_libraries(AikoRenderer PRIVATE volk::volk Vulkan::Headers spirv-reflect-static)
target_link_libraries(imgui PUBLIC volk::volk Vulkan::Headers)
aiko_set_common_source_layout(AikoRenderer ${CMAKE_CURRENT_SOURCE_DIR} ${AIKORENDERER_VULKAN_SRC})
