cmake_minimum_required(VERSION 3.25 FATAL_ERROR)


# Set defaults before project call
if(PROJECT_IS_TOP_LEVEL)
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
endif()


# Create project
project(sourcepp)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Options (update print_options at the bottom of this file when modifying)
option(SOURCEPP_LIBS_START_ENABLED "Libraries will all build by default"                             ON)
option(SOURCEPP_USE_BSPPP          "Build bsppp library"                 ${SOURCEPP_LIBS_START_ENABLED})
option(SOURCEPP_USE_DMXPP          "Build dmxpp library"                 ${SOURCEPP_LIBS_START_ENABLED})
option(SOURCEPP_USE_GAMEPP         "Build gamepp library"                ${SOURCEPP_LIBS_START_ENABLED})
option(SOURCEPP_USE_KVPP           "Build kvpp library"                  ${SOURCEPP_LIBS_START_ENABLED})
option(SOURCEPP_USE_MDLPP          "Build mdlpp library"                 ${SOURCEPP_LIBS_START_ENABLED})
option(SOURCEPP_USE_STEAMPP        "Build steampp library"               ${SOURCEPP_LIBS_START_ENABLED})
option(SOURCEPP_USE_TOOLPP         "Build toolpp library"                ${SOURCEPP_LIBS_START_ENABLED})
option(SOURCEPP_USE_VCRYPTPP       "Build vcryptpp library"              ${SOURCEPP_LIBS_START_ENABLED})
option(SOURCEPP_USE_VPKPP          "Build vpkpp library"                 ${SOURCEPP_LIBS_START_ENABLED})
option(SOURCEPP_USE_VTFPP          "Build vtfpp library"                 ${SOURCEPP_LIBS_START_ENABLED})

option(SOURCEPP_BUILD_BENCHMARKS   "Build benchmarks for supported libraries" OFF)
option(SOURCEPP_BUILD_C_WRAPPERS   "Build C wrappers for supported libraries" OFF)
option(SOURCEPP_BUILD_WITH_OPENCL  "Build with support for GPU compute"       OFF)
option(SOURCEPP_BUILD_WITH_TBB     "Build with support for std::execution"    OFF)
option(SOURCEPP_BUILD_WITH_THREADS "Build with support for std::thread"        ON)
option(SOURCEPP_BUILD_TESTS        "Build tests for supported libraries"      OFF)
option(SOURCEPP_BUILD_WIN7_COMPAT  "Build with Windows 7 compatibility"       OFF)

option(SOURCEPP_LINK_STATIC_MSVC_RUNTIME "Link to static MSVC runtime library" OFF)


# Option overrides
if(SOURCEPP_USE_STEAMPP)
    set(SOURCEPP_USE_KVPP ON CACHE INTERNAL "" FORCE)
endif()
if(SOURCEPP_USE_TOOLPP)
    set(SOURCEPP_USE_KVPP ON CACHE INTERNAL "" FORCE)
endif()
if(SOURCEPP_USE_VPKPP)
    set(SOURCEPP_USE_BSPPP ON CACHE INTERNAL "" FORCE)
    set(SOURCEPP_USE_KVPP ON CACHE INTERNAL "" FORCE)
endif()

if(MSVC)
    # MSVC does not rely on tbb for std::execution policies, so we can force this on
    set(SOURCEPP_BUILD_WITH_TBB ON CACHE INTERNAL "" FORCE)
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
include(PrintOptions)
include(TargetOptimize)


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
            GIT_TAG v1.15.2)
    FetchContent_MakeAvailable(googletest)
    enable_testing()
    set(${SOURCEPP_TEST_NAME}_SOURCES "")
    list(APPEND ${SOURCEPP_TEST_NAME}_DEPS gtest_main)
endif()


# Benchmarks
if(SOURCEPP_BUILD_BENCHMARKS)
    set(SOURCEPP_BENCH_NAME "${PROJECT_NAME}_bench")
    include(FetchContent)
    FetchContent_Declare(
            benchmark
            GIT_REPOSITORY https://github.com/google/benchmark.git
            GIT_TAG v1.9.0)
    set(BENCHMARK_ENABLE_TESTING OFF CACHE INTERNAL "" FORCE)
    FetchContent_MakeAvailable(benchmark)

    add_library(${SOURCEPP_BENCH_NAME} STATIC "${CMAKE_CURRENT_SOURCE_DIR}/bench/_bench.cpp")
    target_link_libraries(${SOURCEPP_BENCH_NAME} PUBLIC benchmark::benchmark)
    target_compile_definitions(${SOURCEPP_BENCH_NAME} PUBLIC ASSET_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/test/res/")
endif()


# Add libraries
add_sourcepp_library(bsppp             NO_TEST      ) # sourcepp::bsppp
add_sourcepp_library(dmxpp                          ) # sourcepp::dmxpp
add_sourcepp_library(gamepp                         ) # sourcepp::gamepp
add_sourcepp_library(kvpp                      BENCH) # sourcepp::kvpp
add_sourcepp_library(mdlpp                          ) # sourcepp::mdlpp
add_sourcepp_library(steampp  C                     ) # sourcepp::steampp
add_sourcepp_library(toolpp                         ) # sourcepp::toolpp
add_sourcepp_library(vcryptpp C CSHARP              ) # sourcepp::vcryptpp
add_sourcepp_library(vpkpp    C CSHARP NO_TEST      ) # sourcepp::vpkpp
add_sourcepp_library(vtfpp                     BENCH) # sourcepp::vtfpp


# Tests, part 2
if(SOURCEPP_BUILD_TESTS)
    add_executable(${SOURCEPP_TEST_NAME} ${${SOURCEPP_TEST_NAME}_SOURCES})
    target_link_libraries(${SOURCEPP_TEST_NAME} PUBLIC ${${SOURCEPP_TEST_NAME}_DEPS})
    target_compile_definitions(${SOURCEPP_TEST_NAME} PUBLIC ASSET_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/test/res/")
    include(GoogleTest)
    gtest_discover_tests(${SOURCEPP_TEST_NAME})
endif()


# Print options
print_options(OPTIONS
        USE_BSPPP USE_DMXPP USE_GAMEPP USE_KVPP USE_MDLPP USE_STEAMPP USE_TOOLPP USE_VCRYPTPP USE_VPKPP USE_VTFPP
        BUILD_BENCHMARKS BUILD_C_WRAPPERS BUILD_WITH_OPENCL BUILD_WITH_TBB BUILD_WITH_THREADS BUILD_TESTS BUILD_WIN7_COMPAT
        LINK_STATIC_MSVC_RUNTIME)
