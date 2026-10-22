include(FetchContent)

#----------------------------------------------------------------------

# ----------------------------------------------------------------------
# Jolt CPU instruction configuration
# ----------------------------------------------------------------------

option(AIKO_PHYSICS_NATIVE_CPU "Optimize Jolt for the CPU of the build machine" ON)

# Portable x86 baseline: SSE2 only.
set(JOLT_CPU_FEATURES
        SSE4_1
        SSE4_2
        AVX
        AVX2
        AVX512
        F16C
        FMADD
        LZCNT
        TZCNT
)

foreach(feature IN LISTS JOLT_CPU_FEATURES)
    set(USE_${feature} OFF CACHE BOOL "" FORCE)
endforeach()

if(AIKO_PHYSICS_NATIVE_CPU
        AND CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64|i[3-6]86)$"
        AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang"
        AND NOT CMAKE_CROSSCOMPILING)

    execute_process(
            COMMAND ${CMAKE_CXX_COMPILER} -march=native -dM -E -x c++ -
            INPUT_FILE /dev/null
            OUTPUT_VARIABLE CPU_DEFINITIONS
            ERROR_QUIET
            RESULT_VARIABLE DETECT_RESULT
    )

    if(DETECT_RESULT EQUAL 0)
        function(aiko_enable_cpu_feature feature macro)
            if(CPU_DEFINITIONS MATCHES "#define ${macro} 1")
                set(USE_${feature} ON CACHE BOOL "" FORCE)
            endif()
        endfunction()

        aiko_enable_cpu_feature(SSE4_1 __SSE4_1__)
        aiko_enable_cpu_feature(SSE4_2 __SSE4_2__)
        aiko_enable_cpu_feature(AVX __AVX__)
        aiko_enable_cpu_feature(AVX2 __AVX2__)
        aiko_enable_cpu_feature(AVX512 __AVX512F__)
        aiko_enable_cpu_feature(F16C __F16C__)
        aiko_enable_cpu_feature(FMADD __FMA__)
        aiko_enable_cpu_feature(LZCNT __LZCNT__)
        aiko_enable_cpu_feature(TZCNT __BMI__)
    else()
        message(WARNING "Unable to detect native CPU features; using portable Jolt baseline"
        )
    endif()
endif()

message(STATUS "Jolt CPU instruction configuration:")
foreach(feature IN LISTS JOLT_CPU_FEATURES)
    message(STATUS "  ${feature}: ${USE_${feature}}")
endforeach()

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

aiko_add_dependency(Jolt)

#----------------------------------------------------------------------
