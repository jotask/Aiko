cmake_minimum_required(VERSION 3.22)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(AikoFramework LANGUAGES CXX)

if (WIN32)
    set(AIKO_CURRENT_PLATFORM AIKO_WINDOWS)
    message("Compiling for Windows")
elseif (UNIX)
    set(AIKO_CURRENT_PLATFORM AIKO_LINUX)
    message("Compiling for linux")
else()
    message(FATAL_ERROR "Unsupported OS")
endif()

add_subdirectory("${CMAKE_SOURCE_DIR}/aiko")
add_subdirectory("${CMAKE_SOURCE_DIR}/examples")

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT AikoEditor )