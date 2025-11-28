include(FetchContent)

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

FetchContent_Declare(
    stb
    GIT_REPOSITORY  https://github.com/nothings/stb.git
    GIT_TAG         master
    GIT_SHALLOW     TRUE
    GIT_PROGRESS    TRUE
)
FetchContent_GetProperties(stb)
if(NOT stb_POPULATED)
    FetchContent_MakeAvailable(stb)
    message("Fetching stb")

    add_library(stb INTERFACE ${stb_SOURCE_DIR})
    target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})
endif()
set_target_properties(stb PROPERTIES FOLDER "Dependencies")

#----------------------------------------------------------------------

set(BGFX_BUILD_TOOLS ON CACHE BOOL "" FORCE)
set(BGFX_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BX_USE_DX ON CACHE BOOL "" FORCE)               # Enable DX/HLSL support
set(BGFX_RENDERER_DIRECT3D11 ON CACHE BOOL "" FORCE)

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
  GIT_TAG master
)

FetchContent_MakeAvailable(bx bimg bgfx)
set_target_properties(bx PROPERTIES FOLDER "Dependencies")
set_target_properties(bimg PROPERTIES FOLDER "Dependencies")
set_target_properties(bgfx PROPERTIES FOLDER "Dependencies")
#----------------------------------------------------------------------
