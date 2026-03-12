include(FetchContent)

# set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/libs CACHE PATH "Folder to cache FetchContent libraries." FORCE)
set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_BINARY_DIR}/libs CACHE PATH "Folder to cache FetchContent libraries." FORCE)
Set(FETCHCONTENT_QUIET FALSE)
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)

#----------------------------------------------------------------------

FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)

set(GLAD_PROFILE "core" CACHE STRING "OpenGL profile")
set(GLAD_API "gl=4.6" CACHE STRING "API type/version pairs, like \"gl=4.6\", no version means latest")
set(GLAD_GENERATOR "c" CACHE STRING "Language to generate the binding for")
set(GLAD_EXTENSIONS "GL_ARB_bindless_texture" CACHE STRING "Extensions to take into consideration when generating the bindings")

FetchContent_GetProperties(glad)

if(NOT glad_POPULATED)
    message("Fetching glad")
    FetchContent_MakeAvailable(glad)
endif()
set_target_properties(glad PROPERTIES FOLDER "Dependencies")
set_target_properties(glad-generate-files PROPERTIES FOLDER "Dependencies")

#----------------------------------------------------------------------

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(GLFW_LIBRARY_TYPE "Link glfw static or dynamic" STATIC)
option(GLFW_BUILD_TESTS "" OFF)
option(GLFW_BUILD_DOCS "" OFF)
option(GLFW_INSTALL "" OFF)
option(GLFW_BUILD_EXAMPLES "" OFF)
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw
        GIT_TAG        3.3.8
        GIT_SHALLOW    TRUE
        GIT_PROGRESS   TRUE
)
message("Fetching glfw")
FetchContent_MakeAvailable(glfw)
set_target_properties(glfw PROPERTIES FOLDER "Dependencies")

#----------------------------------------------------------------------

FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui
        GIT_TAG        docking
        GIT_SHALLOW    TRUE
        GIT_PROGRESS   TRUE
)

FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
    message("Fetching imgui")
    FetchContent_MakeAvailable(imgui)

    add_library(imgui
            ${imgui_SOURCE_DIR}/imgui.cpp
            ${imgui_SOURCE_DIR}/imgui_demo.cpp
            ${imgui_SOURCE_DIR}/imgui_draw.cpp
            ${imgui_SOURCE_DIR}/imgui_widgets.cpp
            ${imgui_SOURCE_DIR}/imgui_tables.cpp
            #${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
            ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp)

    target_include_directories(imgui PUBLIC
            ${imgui_SOURCE_DIR}
            ${imgui_SOURCE_DIR}/backends
            ${glfw_SOURCE_DIR}/include)

    target_link_libraries(imgui PRIVATE glfw)
endif ()
set_target_properties(imgui PROPERTIES FOLDER "Dependencies")

#----------------------------------------------------------------------

set(BGFX_BUILD_TOOLS ON CACHE BOOL "" FORCE)
set(BGFX_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

if (WIN32)
    set(BX_USE_DX ON CACHE BOOL "" FORCE)
    set(BGFX_RENDERER_DIRECT3D11 ON CACHE BOOL "" FORCE)
else()
    set(BX_USE_DX OFF CACHE BOOL "" FORCE)
    set(BGFX_RENDERER_DIRECT3D11 OFF CACHE BOOL "" FORCE)
    set(BGFX_RENDERER_VULKAN ON CACHE BOOL "" FORCE)   # SPIR-V path
endif()

FetchContent_Declare(
  bx
  GIT_REPOSITORY https://github.com/bkaradzic/bx.git
  GIT_TAG master
)

FetchContent_Declare(
  bimg
  GIT_REPOSITORY https://github.com/bkaradzic/bimg.git
  GIT_TAG master
)

FetchContent_Declare(
  bgfx
  GIT_REPOSITORY https://github.com/bkaradzic/bgfx.cmake.git
  GIT_TAG v1.139.9155-513
)

FetchContent_MakeAvailable(bx bimg bgfx)
set_target_properties(bx PROPERTIES FOLDER "Dependencies")
set_target_properties(bimg PROPERTIES FOLDER "Dependencies")
set_target_properties(bgfx PROPERTIES FOLDER "Dependencies")
#----------------------------------------------------------------------
