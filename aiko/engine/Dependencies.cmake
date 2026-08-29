include(FetchContent)

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

# Better than patching -Werror afterward
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(assimp)

if (TARGET assimp)
    target_compile_options(assimp PRIVATE
            -Wno-dangling-reference
            -Wno-error=dangling-reference
    )

    set_target_properties(assimp PROPERTIES FOLDER "Dependencies")
endif()

if (TARGET zlibstatic)
    set_target_properties(zlibstatic PROPERTIES FOLDER "Dependencies")
endif()

if (TARGET UpdateAssimpLibsDebugSymbolsAndDLLs)
    set_target_properties(UpdateAssimpLibsDebugSymbolsAndDLLs PROPERTIES FOLDER "Dependencies")
endif()

#----------------------------------------------------------------------


FetchContent_Declare(
    EnTT
    GIT_REPOSITORY  https://github.com/skypjack/entt.git
    GIT_TAG         v3.16.0
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
