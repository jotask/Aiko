include(FetchContent)

set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_BINARY_DIR}/libs CACHE PATH "Missing description." FORCE)
Set(FETCHCONTENT_QUIET FALSE)

#----------------------------------------------------------------------

set(JSONCPP_WITH_TESTS OFF CACHE BOOL "" FORCE)
set(JSONCPP_WITH_POST_BUILD_UNITTEST OFF CACHE BOOL "" FORCE)
set(JSONCPP_WITH_WARNING_AS_ERROR OFF CACHE BOOL "" FORCE)
set(JSONCPP_WITH_EXAMPLE OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_OBJECT_LIBS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    jsoncpp
    GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp.git
    GIT_TAG        master
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)

message("Fetching jsoncpp")
FetchContent_MakeAvailable(jsoncpp)

#----------------------------------------------------------------------

FetchContent_Declare(
        yaml-cpp
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
        GIT_TAG yaml-cpp-0.9.0
)
FetchContent_MakeAvailable(yaml-cpp)

#----------------------------------------------------------------------

# ImGuiFileDialog
FetchContent_Declare(
        imguifiledialog
        GIT_REPOSITORY https://github.com/aiekick/ImGuiFileDialog.git
        GIT_TAG v0.6.8
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(imguifiledialog)

#----------------------------------------------------------------------