include(Dependencies.cmake)

file(GLOB_RECURSE AIKO_EDITOR_SRC "src/*.h" "src/*.cpp" "src/*.hpp")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${AIKO_EDITOR_SRC})

add_executable(AikoEditor ${AIKO_EDITOR_SRC})
target_link_libraries(AikoEditor PRIVATE Aiko imgui jsoncpp_static yaml-cpp::yaml-cpp)
target_sources(AikoEditor PRIVATE  ${imguifiledialog_SOURCE_DIR}/ImGuiFileDialog.cpp )
target_include_directories(AikoEditor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src ${imguifiledialog_SOURCE_DIR})

aiko_apply_defaults(AikoEditor)