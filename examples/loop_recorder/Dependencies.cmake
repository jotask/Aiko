include(FetchContent)

set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_BINARY_DIR}/libs CACHE PATH "Missing description." FORCE)
Set(FETCHCONTENT_QUIET FALSE)

#----------------------------------------------------------------------

# Fetch PortAudio
FetchContent_Declare(
    PortAudio
    GIT_REPOSITORY https://github.com/PortAudio/portaudio.git
    GIT_TAG        v19.7.0  # Latest stable version
)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build PortAudio as a static library" FORCE)

# Make available
FetchContent_MakeAvailable(PortAudio)

#----------------------------------------------------------------------
