include(Dependencies.cmake)

set(GLOBAL_PATH ${CMAKE_SOURCE_DIR}/)
set(GLOBAL_ASSET_PATH ${CMAKE_SOURCE_DIR}/assets/)
configure_file(constants.h.in ${CMAKE_CURRENT_SOURCE_DIR}/src/constants.h)

set(AIKORENDERER_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(AIKORENDERER_PLATFORM_DIR "${AIKORENDERER_SRC_DIR}/platform")

aiko_collect_target_sources_flat(AIKORENDERER_COMMON_SRC "${AIKORENDERER_SRC_DIR}")

file(GLOB AIKORENDERER_COMMON_DIRS CONFIGURE_DEPENDS LIST_DIRECTORIES true
        "${AIKORENDERER_SRC_DIR}/*"
)

foreach(AIKORENDERER_COMMON_DIR IN LISTS AIKORENDERER_COMMON_DIRS)
    if(IS_DIRECTORY "${AIKORENDERER_COMMON_DIR}" AND NOT "${AIKORENDERER_COMMON_DIR}" STREQUAL "${AIKORENDERER_PLATFORM_DIR}")
        aiko_collect_target_sources(AIKORENDERER_COMMON_DIR_SRC "${AIKORENDERER_COMMON_DIR}")
        list(APPEND AIKORENDERER_COMMON_SRC ${AIKORENDERER_COMMON_DIR_SRC})
    endif()
endforeach()

add_library(AikoRenderer STATIC ${AIKORENDERER_COMMON_SRC})

target_include_directories(AikoRenderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_compile_definitions(AikoRenderer PRIVATE ${AIKO_RENDER})

target_precompile_headers(AikoRenderer PRIVATE
        "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/src/aiko_pch.h>"
)

target_link_libraries(AikoRenderer
        PRIVATE AikoCore glfw imgui assimp Tracy::TracyClient
        PUBLIC magic_enum
)

if(AIKO_RENDER STREQUAL "AIKO_BGFX")
    add_subdirectory(src/platform/bgfx)
elseif(AIKO_RENDER STREQUAL "AIKO_VULKAN")
    add_subdirectory(src/platform/vulkan)
elseif(AIKO_RENDER STREQUAL "AIKO_NATIVE")
    add_subdirectory(src/platform/opengl)
else()
    message(FATAL_ERROR "Unsupported AIKO_RENDER=${AIKO_RENDER}")
endif()

aiko_set_common_source_layout(AikoRenderer ${AIKORENDERER_SRC_DIR} ${AIKORENDERER_COMMON_SRC})
aiko_apply_defaults(AikoRenderer)
