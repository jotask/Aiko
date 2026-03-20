include(Dependencies.cmake)

# =========================
# Library (compiler core)
# =========================

# Files
file(GLOB_RECURSE EXAMPLE_NAIKO_SRC "lib/*.h" "lib/*.cpp" "lib/*.hpp")

# Organize source files into folders
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/lib FILES ${EXAMPLE_NAIKO_SRC})

# Main executable
add_library(NaikoLib ${EXAMPLE_NAIKO_SRC})

target_link_libraries(NaikoLib PUBLIC AikoCore magic_enum llvm_vendor)
target_include_directories(NaikoLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib) #include itself

# =========================
# Main executable
# =========================

# generate constant file
set(NAIKO_ASSET_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src/)
configure_file(constants.h.in ${CMAKE_CURRENT_SOURCE_DIR}/src/constants.h)

file(GLOB_RECURSE EXAMPLE_NAIKO_EXE "src/main.cpp")
add_executable(Naiko ${EXAMPLE_NAIKO_EXE})
target_link_libraries(Naiko PRIVATE NaikoLib)

# =========================
# Tests
# =========================

enable_testing()

# Files
file(GLOB_RECURSE EXAMPLE_NAIKO_TESTS "tests/*.cpp")

add_executable(NaikoTests ${EXAMPLE_NAIKO_TESTS})

target_include_directories(NaikoTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib )

target_link_libraries(NaikoTests PRIVATE NaikoLib Catch2::Catch2WithMain magic_enum llvm_vendor )

include(CTest)
include(Catch)
catch_discover_tests(NaikoTests)
