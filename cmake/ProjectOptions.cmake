function(aiko_setup_options)
    option(AIKO_ENABLE_UNITY "Enable unity/jumbo builds" OFF)
    option(AIKO_BUILD_EXAMPLES "Build example executables" OFF)

    set(AIKO_RENDER "AIKO_BGFX" CACHE STRING "Render backend")
    set_property(CACHE AIKO_RENDER PROPERTY STRINGS AIKO_BGFX AIKO_NATIVE)

    option(AIKO_PROFILER "Enable profiling tools" OFF)

    # Initialize unity build for targets created after this point
    if(AIKO_ENABLE_UNITY)
        set(CMAKE_UNITY_BUILD ON PARENT_SCOPE)
    else()
        set(CMAKE_UNITY_BUILD OFF PARENT_SCOPE)
    endif()
endfunction()

function(aiko_detect_platform)
    if(WIN32)
        set(AIKO_CURRENT_PLATFORM AIKO_WINDOWS PARENT_SCOPE)
    elseif(UNIX)
        set(AIKO_CURRENT_PLATFORM AIKO_LINUX PARENT_SCOPE)
    else()
        message(FATAL_ERROR "Unsupported OS")
    endif()
endfunction()

function(aiko_apply_defaults target_name)

    target_compile_features(${target_name} PUBLIC cxx_std_20)
    target_compile_definitions(${target_name} PUBLIC NOMINMAX)

    if(AIKO_CURRENT_PLATFORM)
        target_compile_definitions(${target_name} PUBLIC ${AIKO_CURRENT_PLATFORM})
    endif()

    if(AIKO_ENABLE_UNITY)
        set_target_properties(${target_name} PROPERTIES UNITY_BUILD ON)
    endif()

    if(AIKO_PROFILER)
        target_compile_definitions(${target_name} PRIVATE AIKO_PROFILER)
        target_compile_definitions(${target_name} PRIVATE AIKO_PROFILE_COPIES)
    endif()

    target_compile_definitions(${target_name} PRIVATE DEBUG)

endfunction()

function(aiko_set_common_source_layout target_name root_dir)
    source_group(TREE ${root_dir} FILES ${ARGN})
endfunction()
