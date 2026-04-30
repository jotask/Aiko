include(FetchContent)

set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_BINARY_DIR}/libs CACHE PATH "Folder to cache FetchContent libraries." FORCE)
Set(FETCHCONTENT_QUIET FALSE)
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)

#----------------------------------------------------------------------

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.10.0
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)

message("Fetching spdlog")
FetchContent_MakeAvailable(spdlog)
set_target_properties(spdlog PROPERTIES FOLDER "Dependencies")

#----------------------------------------------------------------------

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm
    GIT_TAG        1.0.3
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)

message("Fetching glm")
FetchContent_MakeAvailable(glm)
target_compile_definitions(glm INTERFACE GLM_FORCE_SILENT_WARNINGS)
set_target_properties(glm PROPERTIES FOLDER "Dependencies")

#----------------------------------------------------------------------

FetchContent_Declare(
    tracy
    GIT_REPOSITORY  https://github.com/wolfpld/tracy.git
    GIT_TAG         v0.13.1
    GIT_SHALLOW     TRUE
    GIT_PROGRESS    TRUE
)

set(TRACY_ENABLE ${AIKO_PROFILER} CACHE BOOL "Enable profiling" FORCE)
message("Fetching tracy")
FetchContent_MakeAvailable(tracy)
set_target_properties(TracyClient PROPERTIES FOLDER "Dependencies")

if (AIKO_PROFILER)

    set(TRACY_PROFILER_BIN "${tracy_SOURCE_DIR}/profiler/build/tracy-profiler")
    set(TRACY_PROFILER_LAUNCHER "${CMAKE_CURRENT_BINARY_DIR}/run-tracy-profiler.sh")

    file(GENERATE OUTPUT "${TRACY_PROFILER_LAUNCHER}" CONTENT
            "#!/usr/bin/env bash
            set -e
            exec \"${TRACY_PROFILER_BIN}\"
    ")

    add_custom_target(TracyProfilerBuild
            COMMAND ${CMAKE_COMMAND}
            -S ${tracy_SOURCE_DIR}/profiler
            -B ${tracy_SOURCE_DIR}/profiler/build
            -DCMAKE_BUILD_TYPE=Release
            -DLEGACY=ON
            COMMAND ${CMAKE_COMMAND}
            --build ${tracy_SOURCE_DIR}/profiler/build
            --parallel 2
            WORKING_DIRECTORY ${tracy_SOURCE_DIR}
            COMMENT "Building Tracy profiler"
    )

    add_custom_target(TracyProfiler
            COMMAND /bin/bash "${TRACY_PROFILER_LAUNCHER}"
            DEPENDS TracyProfilerBuild
            USES_TERMINAL
            COMMENT "Launching Tracy profiler"
    )

endif()

if (AIKO_PROFILER)

    set(TRACY_CSVEXPORT_BIN "${tracy_SOURCE_DIR}/csvexport/build/tracy-csvexport")
    set(TRACY_CSVEXPORT_LAUNCHER "${CMAKE_CURRENT_BINARY_DIR}/run-tracy-csvexport.sh")

    add_custom_target(TracyCsvExportBuild
            COMMAND ${CMAKE_COMMAND}
            -S ${tracy_SOURCE_DIR}/csvexport
            -B ${tracy_SOURCE_DIR}/csvexport/build
            -DCMAKE_BUILD_TYPE=Release
            COMMAND ${CMAKE_COMMAND}
            --build ${tracy_SOURCE_DIR}/csvexport/build
            --parallel 2
            WORKING_DIRECTORY ${tracy_SOURCE_DIR}
            COMMENT "Building Tracy csvexport"
    )

    file(GENERATE OUTPUT "${TRACY_CSVEXPORT_LAUNCHER}" CONTENT
            "#!/usr/bin/env bash
            set -euo pipefail

            if [ \"$#\" -lt 2 ]; then
                echo \"Usage: $0 <input.tracy> <output.csv>\" >&2
                exit 1
            fi

            input=\"$1\"
            output=\"$2\"

            \"${TRACY_CSVEXPORT_BIN}\" -u \"$input\" > \"$output\"
            echo \"Wrote $output\"
            ")

endif()

#----------------------------------------------------------------------