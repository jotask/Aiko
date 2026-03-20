cmake_minimum_required(VERSION 3.22)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(AikoFramework LANGUAGES CXX)

include(cmake/ProjectOptions.cmake)
aiko_setup_options()
aiko_detect_platform()

add_subdirectory(aiko)

if(AIKO_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT AikoEditor )
