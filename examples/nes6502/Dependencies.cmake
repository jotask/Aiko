include(FetchContent)

set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_BINARY_DIR}/libs CACHE PATH "Missing description." FORCE)
Set(FETCHCONTENT_QUIET FALSE)

#----------------------------------------------------------------------

FetchContent_Declare(
    catch
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v1.10.0
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)

message("Fetching catch")
FetchContent_MakeAvailable(catch)

#----------------------------------------------------------------------
