include(FetchContent)

set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_BINARY_DIR}/libs CACHE PATH "Missing description." FORCE)
Set(FETCHCONTENT_QUIET FALSE)

#----------------------------------------------------------------------

FetchContent_Declare(
  perlinnoise
  GIT_REPOSITORY https://github.com/Reputeless/PerlinNoise.git
  GIT_TAG v3.0.0
  SOURCE_SUBDIR _unused   # optional, prevents add_subdirectory() even if upstream changes
)

FetchContent_MakeAvailable(perlinnoise)

add_library(PerlinNoise INTERFACE)
add_library(PerlinNoise::PerlinNoise ALIAS PerlinNoise)
target_include_directories(PerlinNoise INTERFACE "${perlinnoise_SOURCE_DIR}" )

#----------------------------------------------------------------------
