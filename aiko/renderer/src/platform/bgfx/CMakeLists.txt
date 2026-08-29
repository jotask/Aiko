set(BGFX_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(BGFX_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

if(WIN32)
    set(BX_USE_DX ON CACHE BOOL "" FORCE)
    set(BGFX_RENDERER_DIRECT3D11 ON CACHE BOOL "" FORCE)
else()
    set(BX_USE_DX OFF CACHE BOOL "" FORCE)
    set(BGFX_RENDERER_DIRECT3D11 OFF CACHE BOOL "" FORCE)
    set(BGFX_RENDERER_VULKAN ON CACHE BOOL "" FORCE)
endif()

FetchContent_Declare(
        bgfx
        GIT_REPOSITORY https://github.com/bkaradzic/bgfx.cmake.git
        GIT_TAG v1.139.9155-513
        GIT_SUBMODULES_RECURSE TRUE
)

FetchContent_MakeAvailable(bgfx)

aiko_collect_target_sources(AIKORENDERER_BGFX_SRC "${CMAKE_CURRENT_SOURCE_DIR}")

target_sources(AikoRenderer PRIVATE ${AIKORENDERER_BGFX_SRC})
target_link_libraries(AikoRenderer PRIVATE bx bimg bgfx)
aiko_set_common_source_layout(AikoRenderer ${CMAKE_CURRENT_SOURCE_DIR} ${AIKORENDERER_BGFX_SRC})

set_target_properties(bx bimg bgfx PROPERTIES FOLDER "Dependencies")
