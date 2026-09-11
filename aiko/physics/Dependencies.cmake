include(FetchContent)

#----------------------------------------------------------------------

# Jolt CPU feature overrides for Intel i5-3570
set(USE_SSE4_1 ON  CACHE BOOL "" FORCE)
set(USE_SSE4_2 ON  CACHE BOOL "" FORCE)
set(USE_AVX    ON  CACHE BOOL "" FORCE)
set(USE_F16C   ON  CACHE BOOL "" FORCE)

set(USE_AVX2   OFF CACHE BOOL "" FORCE)
set(USE_FMADD  OFF CACHE BOOL "" FORCE)
set(USE_LZCNT  OFF CACHE BOOL "" FORCE)
set(USE_TZCNT  OFF CACHE BOOL "" FORCE)

# safest temporary debug option:
# set(USE_AVX    OFF CACHE BOOL "" FORCE)
# set(USE_F16C   OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
        JoltPhysics
        GIT_REPOSITORY https://github.com/jrouwe/JoltPhysics
        GIT_TAG v5.5.0
        SOURCE_SUBDIR  Build
        GIT_SHALLOW    TRUE
        GIT_PROGRESS   TRUE
)
message("Fetching JoltPhysics")

if(NOT AIKO_ENGINE_DEBUG)
    set(USE_ASSERTS OFF CACHE BOOL "" FORCE)
    set(DEBUG_RENDERER_IN_DEBUG_AND_RELEASE OFF CACHE BOOL "" FORCE)
    set(PROFILER_IN_DEBUG_AND_RELEASE OFF CACHE BOOL "" FORCE)
endif()

FetchContent_MakeAvailable(JoltPhysics)

if(NOT AIKO_ENGINE_DEBUG)
    aiko_apply_optimized_runtime(Jolt)
    target_compile_definitions(Jolt INTERFACE NDEBUG)
endif()

#----------------------------------------------------------------------
