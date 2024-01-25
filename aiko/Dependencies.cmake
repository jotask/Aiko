cmake_minimum_required(VERSION 3.19)

#raylib
add_subdirectory("${CMAKE_SOURCE_DIR}/submodules/raylib" raylib EXCLUDE_FROM_ALL)

#imgui

project(imgui)

set(imgui_CPP_SRC_DIR "${CMAKE_SOURCE_DIR}/submodules/imgui")

set(imgui_BUILD_SOURCES
    "${imgui_CPP_SRC_DIR}/imgui.cpp"
    "${imgui_CPP_SRC_DIR}/imgui_demo.cpp"
    "${imgui_CPP_SRC_DIR}/imgui_draw.cpp"
    "${imgui_CPP_SRC_DIR}/imgui_tables.cpp"
    "${imgui_CPP_SRC_DIR}/imgui_widgets.cpp"
    "${imgui_CPP_SRC_DIR}/misc/cpp/imgui_stdlib.cpp"
    )

add_library(imgui STATIC "${imgui_BUILD_SOURCES}")
#target_compile_definitions(imgui PUBLIC IMGUI_USER_CONFIG="imgui_user_config.h")
target_compile_definitions(imgui PRIVATE $<$<CONFIG:Release>:IMGUI_DISABLE_DEMO_WINDOWS> $<$<CONFIG:Release>:IMGUI_DISABLE_DEBUG_TOOLS>)
target_include_directories(imgui PUBLIC "${imgui_CPP_SRC_DIR}")

add_executable("binary_to_compressed_c" "${imgui_CPP_SRC_DIR}/misc/fonts/binary_to_compressed_c.cpp")

# RayLib Imgui
project(rlImGui)

set(rlImGui_CPP_SRC_DIR "${CMAKE_SOURCE_DIR}/submodules/rlImGui")
set(rlImGui_BUILD_SOURCES "${rlImGui_CPP_SRC_DIR}/rlImGui.cpp" )

add_library(rlImGui STATIC "${rlImGui_BUILD_SOURCES}")
target_compile_definitions(rlImGui PUBLIC NO_FONT_AWESOME)
target_include_directories(rlImGui PUBLIC "${rlImGui_CPP_SRC_DIR}")
target_link_libraries(rlImGui PUBLIC imgui raylib)

# spdlog
add_subdirectory("${CMAKE_SOURCE_DIR}/submodules/spdlog" spdlog EXCLUDE_FROM_ALL)

# GLM
add_subdirectory("${CMAKE_SOURCE_DIR}/submodules/glm" GLM EXCLUDE_FROM_ALL)
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/submodules/glm")

## Add libraries into folder
set_property(TARGET raylib PROPERTY FOLDER "Submodules")
set_property(TARGET spdlog PROPERTY FOLDER "Submodules")
set_property(TARGET imgui PROPERTY FOLDER "Submodules")
set_property(TARGET rlImGui PROPERTY FOLDER "Submodules")
set_property(TARGET binary_to_compressed_c PROPERTY FOLDER "Submodules")