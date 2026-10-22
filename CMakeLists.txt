cmake_minimum_required(VERSION 3.22)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(AikoFramework LANGUAGES C CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(FetchContent)

set(FETCHCONTENT_BASE_DIR "${PROJECT_BINARY_DIR}/_deps" CACHE PATH "Shared FetchContent directory for this build tree" FORCE)
set(FETCHCONTENT_QUIET FALSE CACHE BOOL "Print FetchContent progress" FORCE)
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE CACHE BOOL "Do not update already-populated FetchContent dependencies" FORCE)

include(cmake/ProjectOptions.cmake)
include(cmake/AikoTargets.cmake)

aiko_setup_options()
aiko_detect_platform()

if(AIKO_RENDER STREQUAL "AIKO_VULKAN")
    set(AIKO_ENGINE_SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders/vulkan")
else()
    message(FATAL_ERROR "Engine shader compilation is not implemented for '${AIKO_RENDER}'")
endif()

aiko_add_shader_target(AikoEngineShaders SOURCE_DIR "${AIKO_ENGINE_SHADER_SOURCE_DIR}" BINARY_DIR "${CMAKE_BINARY_DIR}/assets/shaders")

add_subdirectory(aiko)

if(TARGET AikoEngineShaders)
    add_dependencies(Aiko AikoEngineShaders)
    add_dependencies(AikoRenderer AikoEngineShaders)

    if(TARGET AikoEditor)
        add_dependencies(AikoEditor AikoEngineShaders)
    endif()

    aiko_add_target(AikoEngineShaders)
endif()

if(AIKO_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT AikoEditor )

if(TARGET uninstall)
    aiko_add_dependency(uninstall)
endif()