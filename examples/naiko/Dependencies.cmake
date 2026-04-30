include(FetchContent)

set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_BINARY_DIR}/libs CACHE PATH "Missing description." FORCE)
Set(FETCHCONTENT_QUIET FALSE)

#----------------------------------------------------------------------

FetchContent_Declare(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.12.0
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)

FetchContent_MakeAvailable(catch2)

#----------------------------------------------------------------------

FetchContent_Declare(
    llvm
    GIT_REPOSITORY https://github.com/llvm/llvm-project.git
    GIT_TAG llvmorg-21.1.8
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)

# Configure LLVM options before adding

set(LLVM_ENABLE_PROJECTS "llvm;clang" CACHE STRING "" FORCE)
set(LLVM_TARGETS_TO_BUILD "X86;ARM" CACHE STRING "" FORCE)
set(LLVM_ENABLE_RTTI ON CACHE BOOL "" FORCE)
set(LLVM_ENABLE_EH ON CACHE BOOL "" FORCE)
set(LLVM_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(LLVM_BUILD_UTILS OFF CACHE BOOL "" FORCE)
set(LLVM_INCLUDE_TESTS OFF CACHE BOOL "" FORCE)
set(LLVM_INCLUDE_EXAMPLES OFF CACHE BOOL "" FORCE)
set(LLVM_INCLUDE_DOCS OFF CACHE BOOL "" FORCE)
# Fetch

FetchContent_MakeAvailable(llvm)

# Build LLVM from the llvm subfolder
add_subdirectory(${llvm_SOURCE_DIR}/llvm ${llvm_BINARY_DIR})

# Create interface target
add_library(llvm_vendor INTERFACE)

target_include_directories(llvm_vendor INTERFACE
        ${llvm_SOURCE_DIR}/llvm/include
        ${llvm_BINARY_DIR}/include
        ${llvm_BINARY_DIR}/llvm/include
        ${llvm_BINARY_DIR}/llvm/include/llvm/Config
)

target_link_libraries(llvm_vendor INTERFACE
        LLVMCore
        LLVMSupport
        LLVMIRReader
        LLVMTarget
        ## x86_64
        LLVMX86Info
        LLVMX86Desc
        LLVMX86CodeGen
        LLVMX86AsmParser
        ## ARM
        LLVMARMInfo
        LLVMARMDesc
        LLVMARMCodeGen
        # Shared
        LLVMAsmPrinter

)

#----------------------------------------------------------------------
