include(FetchContent)

set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_BINARY_DIR}/libs CACHE PATH "Folder to cache FetchContent libraries." FORCE)
Set(FETCHCONTENT_QUIET FALSE)
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)

#----------------------------------------------------------------------

FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG        v5.3.1
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)
message("Fetching assimp")
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(assimp)
if (TARGET assimp)
    target_compile_options(assimp PRIVATE
            -Wno-error
            -Wno-dangling-reference
            -Wno-error=dangling-reference
    )
endif()
set_target_properties(assimp PROPERTIES FOLDER "Dependencies")
set_target_properties(zlibstatic PROPERTIES FOLDER "Dependencies")
if (TARGET UpdateAssimpLibsDebugSymbolsAndDLLs)
    set_target_properties(UpdateAssimpLibsDebugSymbolsAndDLLs PROPERTIES FOLDER "Dependencies")
endif()

#----------------------------------------------------------------------

FetchContent_Declare(
    tracy
    GIT_REPOSITORY  https://github.com/wolfpld/tracy.git
    GIT_TAG         master
    GIT_SHALLOW     TRUE
    GIT_PROGRESS    TRUE
)

set(TRACY_ENABLE OFF CACHE BOOL "Enable profiling")
#set(TRACY_NO_SYSTEM_TRACING ON CACHE BOOL "Disable System Tracing")
set(TRACY_ONLY_IPV4 OFF CACHE BOOL "IPv4 only")
option(TRACY_ENABLE "Enable profiling" OFF)
#option(TRACY_NO_SYSTEM_TRACING "Disable System Tracing" ON)
option(TRACY_ONLY_IPV4 "IPv4 only" OFF)
message("Fetching tracy")
FetchContent_MakeAvailable(tracy)
set_target_properties(TracyClient PROPERTIES FOLDER "Dependencies")

#----------------------------------------------------------------------

FetchContent_Declare(
    EnTT
    GIT_REPOSITORY  https://github.com/skypjack/entt.git
    GIT_TAG         main
    GIT_SHALLOW     TRUE
    GIT_PROGRESS    TRUE
)
message("Fetching EnTT")
FetchContent_MakeAvailable(EnTT)
target_compile_definitions(EnTT INTERFACE ENTT_FORCE_SILENT_WARNINGS)
set_target_properties(EnTT PROPERTIES FOLDER "Dependencies")

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

FetchContent_Declare(
    magic_enum
    GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
    GIT_TAG        v0.9.7
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)

message("Fetching magic_enum")
FetchContent_MakeAvailable(magic_enum)

#----------------------------------------------------------------------
