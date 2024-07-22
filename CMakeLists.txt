cmake_minimum_required(VERSION 3.28 FATAL_ERROR)


# Set defaults before project call
if(PROJECT_IS_TOP_LEVEL)
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
endif()


# Create project
project(sourcepp)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# Options
option(SOURCEPP_USE_BSPPP               "Build bsppp library"                       ON)
option(SOURCEPP_USE_DMXPP               "Build dmxpp library"                       ON)
option(SOURCEPP_USE_FGDPP               "Build fgdpp library"                       ON)
option(SOURCEPP_USE_KVPP                "Build kvpp library"                        ON)
option(SOURCEPP_USE_MDLPP               "Build mdlpp library"                       ON)
option(SOURCEPP_USE_STEAMPP             "Build steampp library"                     ON)
option(SOURCEPP_USE_VICEPP              "Build vicepp library"                      ON)
option(SOURCEPP_USE_VPKPP               "Build vpkpp library"                       ON)
option(SOURCEPP_USE_VTFPP               "Build vtfpp library"                       ON)
option(SOURCEPP_BUILD_C_WRAPPERS        "Build C wrappers for supported libraries"  ON)
option(SOURCEPP_BUILD_TESTS             "Build tests for enabled libraries"        OFF)
option(SOURCEPP_USE_STATIC_MSVC_RUNTIME "Link to static MSVC runtime library"      OFF)


# Option overrides
if(SOURCEPP_USE_STEAMPP)
    set(SOURCEPP_USE_KVPP ON CACHE INTERNAL "")
endif()
if(SOURCEPP_USE_VPKPP)
    set(SOURCEPP_USE_BSPPP ON CACHE INTERNAL "")
    set(SOURCEPP_USE_KVPP ON CACHE INTERNAL "")
endif()


# Set defaults after project call
if(PROJECT_IS_TOP_LEVEL)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

if(SOURCEPP_USE_STATIC_MSVC_RUNTIME)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()


# Add scripts
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(AddPrettyParser)
include(AddSourcePPLibrary)
include(IncludeSubdirectory)


# Include thirdparty libraries
include_subdirectory(ext)


# Shared code
include_subdirectory(src/sourcepp)


# Shared C code
if(SOURCEPP_BUILD_C_WRAPPERS)
    include_subdirectory(lang/c/src/sourceppc PROPAGATE ${PROJECT_NAME}c_SOURCES)
endif()


# Tests, part 1
if(SOURCEPP_BUILD_TESTS)
    set(SOURCEPP_TEST_NAME "${PROJECT_NAME}_test")
    include(FetchContent)
    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.14.0)
    FetchContent_MakeAvailable(googletest)
    enable_testing()
    set(${SOURCEPP_TEST_NAME}_SOURCES "")
    list(APPEND ${SOURCEPP_TEST_NAME}_DEPS gtest_main)
endif()


# Add libraries
add_sourcepp_library(bsppp NO_TEST)
add_sourcepp_library(dmxpp)
add_sourcepp_library(fgdpp)
add_sourcepp_library(kvpp)
add_sourcepp_library(mdlpp)
add_sourcepp_library(steampp C)
add_sourcepp_library(vicepp C CSHARP)
add_sourcepp_library(vpkpp C CSHARP NO_TEST)
add_sourcepp_library(vtfpp)


# Tests, part 2
if(SOURCEPP_BUILD_TESTS)
    add_executable(${SOURCEPP_TEST_NAME} ${${SOURCEPP_TEST_NAME}_SOURCES})
    target_link_libraries(${SOURCEPP_TEST_NAME} PUBLIC ${${SOURCEPP_TEST_NAME}_DEPS})
    target_compile_definitions(${SOURCEPP_TEST_NAME} PUBLIC ASSET_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/test/res/")
    include(GoogleTest)
    gtest_discover_tests(${SOURCEPP_TEST_NAME})
endif()
