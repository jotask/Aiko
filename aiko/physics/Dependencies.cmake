include(FetchContent)

set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_BINARY_DIR}/libs CACHE PATH "Folder to cache FetchContent libraries." FORCE)
Set(FETCHCONTENT_QUIET FALSE)
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)

#----------------------------------------------------------------------

FetchContent_Declare(
        JoltPhysics
        GIT_REPOSITORY https://github.com/jrouwe/JoltPhysics
        GIT_TAG "v5.5.0"
        SOURCE_SUBDIR  Build
        GIT_SHALLOW    TRUE
        GIT_PROGRESS   TRUE
)
message("Fetching JoltPhysics")
FetchContent_MakeAvailable(JoltPhysics)

#----------------------------------------------------------------------