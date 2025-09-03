include(FetchContent)

set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_BINARY_DIR}/libs CACHE PATH "Missing description." FORCE)
Set(FETCHCONTENT_QUIET FALSE)

#----------------------------------------------------------------------

# Fetch PortAudio

set(PORTAUDIO_BUILD_ASIO OFF CACHE BOOL "Disable ASIO support" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build PortAudio as a static library" FORCE)

FetchContent_Declare(
    PortAudio
    GIT_REPOSITORY https://github.com/PortAudio/portaudio.git
    GIT_TAG        v19.7.0  # Latest stable version
)

# Make available
FetchContent_MakeAvailable(PortAudio)

#----------------------------------------------------------------------
