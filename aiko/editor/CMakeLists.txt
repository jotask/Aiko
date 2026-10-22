include(Dependencies.cmake)

aiko_collect_target_sources(AIKO_EDITOR_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${AIKO_EDITOR_SRC})

add_executable(AikoEditor ${AIKO_EDITOR_SRC})
target_link_libraries(AikoEditor PRIVATE Aiko imgui jsoncpp_static yaml-cpp::yaml-cpp magic_enum)
target_sources(AikoEditor PRIVATE  ${imguifiledialog_SOURCE_DIR}/ImGuiFileDialog.cpp ${imguizmo_SOURCE_DIR}/src/ImGuizmo.cpp)
target_include_directories(AikoEditor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR} ${imguifiledialog_SOURCE_DIR} ${imguizmo_SOURCE_DIR}/src)

aiko_apply_defaults(AikoEditor)
aiko_configure_project_assets(AikoEditor)
aiko_add_target(AikoEditor)
