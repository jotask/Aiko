include(Dependencies.cmake)

# =========================
# Library (compiler core)
# =========================

aiko_collect_target_sources(EXAMPLE_NAIKO_SRC "${CMAKE_CURRENT_SOURCE_DIR}/lib")

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

aiko_collect_target_sources(EXAMPLE_NAIKO_EXE "${CMAKE_CURRENT_SOURCE_DIR}/src")
add_executable(Naiko ${EXAMPLE_NAIKO_EXE})
target_link_libraries(Naiko PRIVATE NaikoLib)

# =========================
# Tests
# =========================

enable_testing()

aiko_collect_target_sources(EXAMPLE_NAIKO_TESTS "${CMAKE_CURRENT_SOURCE_DIR}/tests")

add_executable(NaikoTests ${EXAMPLE_NAIKO_TESTS})

target_include_directories(NaikoTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib )

target_link_libraries(NaikoTests PRIVATE NaikoLib Catch2::Catch2WithMain magic_enum llvm_vendor )

include(CTest)
include(Catch)
catch_discover_tests(NaikoTests)
