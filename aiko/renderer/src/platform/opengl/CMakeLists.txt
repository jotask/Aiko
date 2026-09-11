FetchContent_Declare(
        glad
        GIT_REPOSITORY https://github.com/Dav1dde/glad.git
        GIT_TAG v2.0.8
        GIT_SHALLOW TRUE
        SOURCE_SUBDIR cmake
)

FetchContent_MakeAvailable(glad)

if(NOT TARGET glad)
    glad_add_library(
            glad
            STATIC
            REPRODUCIBLE
            API gl:core=4.6
            EXTENSIONS GL_ARB_bindless_texture
    )
endif()

aiko_collect_target_sources(AIKORENDERER_OPENGL_SRC "${CMAKE_CURRENT_SOURCE_DIR}")

target_sources(AikoRenderer PRIVATE ${AIKORENDERER_OPENGL_SRC})
target_sources(imgui PRIVATE ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp)

target_link_libraries(AikoRenderer PRIVATE glad)
target_link_libraries(imgui PRIVATE glad)
aiko_set_common_source_layout(AikoRenderer ${CMAKE_CURRENT_SOURCE_DIR} ${AIKORENDERER_OPENGL_SRC})

set_target_properties(glad PROPERTIES FOLDER "Dependencies")
