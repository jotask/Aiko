function(aiko_setup_options)
    option(AIKO_ENABLE_UNITY "Enable unity/jumbo builds" OFF)
    option(AIKO_BUILD_EXAMPLES "Build example executables" OFF)
    if(AIKO_BUILD_EXAMPLES)
        option(AIKO_BUILD_EXAMPLES_NAIKO_LLVM "Build example executables" OFF)
    endif()

    set(AIKO_RENDER "AIKO_VULKAN" CACHE STRING "Render backend")
    set_property(CACHE AIKO_RENDER PROPERTY STRINGS AIKO_BGFX AIKO_NATIVE AIKO_VULKAN)

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

function(aiko_collect_target_sources output_variable root_dir)
    if(NOT IS_DIRECTORY "${root_dir}")
        message(FATAL_ERROR "Source root does not exist: ${root_dir}")
    endif()

    file(GLOB_RECURSE collected_sources CONFIGURE_DEPENDS LIST_DIRECTORIES false "${root_dir}/*")
    list(FILTER collected_sources INCLUDE REGEX "\\.(c|cc|cxx|cpp|h|hh|hxx|hpp|inl|ipp)$")

    set(${output_variable} ${collected_sources} PARENT_SCOPE)
endfunction()

function(aiko_collect_target_sources_flat output_variable root_dir)
    if(NOT IS_DIRECTORY "${root_dir}")
        message(FATAL_ERROR "Source root does not exist: ${root_dir}")
    endif()

    file(GLOB collected_sources CONFIGURE_DEPENDS LIST_DIRECTORIES false "${root_dir}/*")
    list(FILTER collected_sources INCLUDE REGEX "\\.(c|cc|cxx|cpp|h|hh|hxx|hpp|inl|ipp)$")

    set(${output_variable} ${collected_sources} PARENT_SCOPE)
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

function(aiko_add_vulkan_shader_target target_name)
    set(oneValueArgs SOURCE_DIR BINARY_DIR TARGET_ENV)
    cmake_parse_arguments(AIKO_SHADER "" "${oneValueArgs}" "" ${ARGN})

    if(NOT AIKO_SHADER_TARGET_ENV)
        set(AIKO_SHADER_TARGET_ENV vulkan1.0)
    endif()

    find_program(GLSLC_EXECUTABLE
            NAMES glslc
            HINTS "$ENV{VULKAN_SDK}/bin" "$ENV{VULKAN_SDK}/Bin"
    )

    if(NOT GLSLC_EXECUTABLE)
        message(FATAL_ERROR "glslc not found. Install Vulkan SDK or shaderc tools.")
    endif()

    file(MAKE_DIRECTORY "${AIKO_SHADER_BINARY_DIR}")

    file(GLOB_RECURSE AIKO_VULKAN_GLSL_SHADERS CONFIGURE_DEPENDS
            "${AIKO_SHADER_SOURCE_DIR}/*.vert"
            "${AIKO_SHADER_SOURCE_DIR}/*.frag"
            "${AIKO_SHADER_SOURCE_DIR}/*.comp"
            "${AIKO_SHADER_SOURCE_DIR}/*.geom"
            "${AIKO_SHADER_SOURCE_DIR}/*.tesc"
            "${AIKO_SHADER_SOURCE_DIR}/*.tese"
    )

    set(AIKO_VULKAN_SPIRV_SHADERS)

    foreach(SHADER ${AIKO_VULKAN_GLSL_SHADERS})
        file(RELATIVE_PATH SHADER_REL "${AIKO_SHADER_SOURCE_DIR}" "${SHADER}")

        set(SPIRV_FILE "${AIKO_SHADER_BINARY_DIR}/${SHADER_REL}.spv")
        set(DEP_FILE "${SPIRV_FILE}.d")

        get_filename_component(SPIRV_DIR "${SPIRV_FILE}" DIRECTORY)

        add_custom_command(
                OUTPUT "${SPIRV_FILE}"
                COMMAND ${CMAKE_COMMAND} -E make_directory "${SPIRV_DIR}"
                COMMAND "${GLSLC_EXECUTABLE}"
                "--target-env=${AIKO_SHADER_TARGET_ENV}"
                -I "${AIKO_SHADER_SOURCE_DIR}"
                -MD -MF "${DEP_FILE}"
                "${SHADER}"
                -o "${SPIRV_FILE}"
                DEPENDS "${SHADER}"
                DEPFILE "${DEP_FILE}"
                COMMENT "Compiling Vulkan shader ${SHADER_REL}"
                VERBATIM
        )

        list(APPEND AIKO_VULKAN_SPIRV_SHADERS "${SPIRV_FILE}")
    endforeach()

    add_custom_target(${target_name}
            ALL
            DEPENDS ${AIKO_VULKAN_SPIRV_SHADERS}
    )
endfunction()
