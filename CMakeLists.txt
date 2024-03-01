cmake_minimum_required(VERSION 3.22)

project(AikoFramework LANGUAGES CXX)

add_subdirectory("${CMAKE_SOURCE_DIR}/aiko")
add_subdirectory("${CMAKE_SOURCE_DIR}/aiko_editor")
add_subdirectory("${CMAKE_SOURCE_DIR}/examples")

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT AikoEditor )