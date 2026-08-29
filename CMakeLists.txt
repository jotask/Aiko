cmake_minimum_required(VERSION 3.22)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(AikoFramework LANGUAGES C CXX)

include(FetchContent)

set(FETCHCONTENT_BASE_DIR "${PROJECT_BINARY_DIR}/_deps" CACHE PATH "Shared FetchContent directory for this build tree" FORCE)
set(FETCHCONTENT_QUIET FALSE CACHE BOOL "Print FetchContent progress" FORCE)
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE CACHE BOOL "Do not update already-populated FetchContent dependencies" FORCE)

include(cmake/ProjectOptions.cmake)
aiko_setup_options()
aiko_detect_platform()

if(AIKO_RENDER STREQUAL "AIKO_VULKAN")
    aiko_add_vulkan_shader_target(AikoVulkanShaders
            SOURCE_DIR "${CMAKE_SOURCE_DIR}/assets/shaders/vulkan"
            BINARY_DIR "${CMAKE_SOURCE_DIR}/build/assets/shaders/vulkan"
            TARGET_ENV vulkan1.0
    )
endif()

add_subdirectory(aiko)

if(TARGET AikoVulkanShaders)
    if(TARGET Aiko)
        add_dependencies(Aiko AikoVulkanShaders)
    endif()

    if(TARGET AikoRenderer)
        add_dependencies(AikoRenderer AikoVulkanShaders)
    endif()

    if(TARGET AikoEditor)
        add_dependencies(AikoEditor AikoVulkanShaders)
    endif()
endif()

if(AIKO_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT AikoEditor )
