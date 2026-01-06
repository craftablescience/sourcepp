cmake_minimum_required(VERSION 3.28 FATAL_ERROR)


# Create project
project(sourcepp
        DESCRIPTION "Several modern C++20 libraries for sanely parsing Valve formats."
        HOMEPAGE_URL "https://sourcepp.org")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Add scripts
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(AddPrettyParser)
include(AddRemoteLibrary)
include(AddSourcePPLibrary)
include(IncludeSubdirectory)
include(PrintOptions)
include(TargetOptimize)


# Options (update print_options at the bottom of this file when modifying)
option(SOURCEPP_LIBS_START_ENABLED "Libraries will all build by default"                             ON)
option(SOURCEPP_USE_BSPPP          "Build bsppp library"                 ${SOURCEPP_LIBS_START_ENABLED})
option(SOURCEPP_USE_DMXPP          "Build dmxpp library"                 ${SOURCEPP_LIBS_START_ENABLED})
option(SOURCEPP_USE_FSPP           "Build fspp library"                  ${SOURCEPP_LIBS_START_ENABLED})
option(SOURCEPP_USE_GAMEPP         "Build gamepp library"                ${SOURCEPP_LIBS_START_ENABLED})
option(SOURCEPP_USE_KVPP           "Build kvpp library"                  ${SOURCEPP_LIBS_START_ENABLED})
option(SOURCEPP_USE_MDLPP          "Build mdlpp library"                 ${SOURCEPP_LIBS_START_ENABLED})
option(SOURCEPP_USE_SNDPP          "Build sndpp library"                 ${SOURCEPP_LIBS_START_ENABLED})
option(SOURCEPP_USE_STEAMPP        "Build steampp library"               ${SOURCEPP_LIBS_START_ENABLED})
option(SOURCEPP_USE_TOOLPP         "Build toolpp library"                ${SOURCEPP_LIBS_START_ENABLED})
option(SOURCEPP_USE_VCRYPTPP       "Build vcryptpp library"              ${SOURCEPP_LIBS_START_ENABLED})
option(SOURCEPP_USE_VPKPP          "Build vpkpp library"                 ${SOURCEPP_LIBS_START_ENABLED})
option(SOURCEPP_USE_VTFPP          "Build vtfpp library"                 ${SOURCEPP_LIBS_START_ENABLED})

option(SOURCEPP_BUILD_C_WRAPPERS      "Build C wrappers for supported libraries"      OFF)
option(SOURCEPP_BUILD_CSHARP_WRAPPERS "Build C# wrappers for supported libraries"     OFF)
option(SOURCEPP_BUILD_PYTHON_WRAPPERS "Build Python wrappers for supported libraries" OFF)
option(SOURCEPP_BUILD_WASM_WRAPPERS   "Build WASM wrappers for supported libraries"   OFF)

option(SOURCEPP_BUILD_BENCHMARKS         "Build benchmarks for supported libraries"  OFF)
option(SOURCEPP_BUILD_WITH_TBB           "Build with support for std::execution"     OFF)
option(SOURCEPP_BUILD_WITH_THREADS       "Build with support for threading"           ON)
option(SOURCEPP_BUILD_TESTS              "Build tests for supported libraries"       OFF)
option(SOURCEPP_BUILD_TESTS_EXTRA        "Build extra tests that can't be run in CI" OFF)
option(SOURCEPP_BUILD_WIN7_COMPAT        "Build with Windows 7 compatibility"         ON)
option(SOURCEPP_LINK_STATIC_MSVC_RUNTIME "Link to static MSVC runtime library"       OFF)


# Options (library-specific)
option(SOURCEPP_VPKPP_SUPPORT_VPK_V54 "Support compressed v54 VPKs" ON)

option(SOURCEPP_VTFPP_SUPPORT_EXR  "Support EXR image format" ON)
option(SOURCEPP_VTFPP_SUPPORT_QOI  "Support QOI image format" ON)
option(SOURCEPP_VTFPP_SUPPORT_WEBP "Support WebP image format" ON)


# Option overrides
if(SOURCEPP_USE_BSPPP)
    set(SOURCEPP_USE_VPKPP ON CACHE INTERNAL "" FORCE)
endif()
if(SOURCEPP_USE_FSPP)
    set(SOURCEPP_USE_BSPPP   ON CACHE INTERNAL "" FORCE)
    set(SOURCEPP_USE_KVPP    ON CACHE INTERNAL "" FORCE)
    set(SOURCEPP_USE_STEAMPP ON CACHE INTERNAL "" FORCE)
    set(SOURCEPP_USE_VPKPP   ON CACHE INTERNAL "" FORCE)
endif()
if(SOURCEPP_USE_STEAMPP)
    set(SOURCEPP_USE_KVPP ON CACHE INTERNAL "" FORCE)
endif()
if(SOURCEPP_USE_TOOLPP)
    set(SOURCEPP_USE_KVPP ON CACHE INTERNAL "" FORCE)
endif()
if(SOURCEPP_USE_VPKPP)
    set(SOURCEPP_USE_KVPP ON CACHE INTERNAL "" FORCE)
endif()

if(EMSCRIPTEN)
    # Don't generate any other bindings besides JS
    set(SOURCEPP_BUILD_CSHARP_WRAPPERS OFF CACHE INTERNAL "" FORCE)
    set(SOURCEPP_BUILD_PYTHON_WRAPPERS OFF CACHE INTERNAL "" FORCE)
    set(SOURCEPP_BUILD_WASM_WRAPPERS    ON CACHE INTERNAL "" FORCE)
else()
    if(SOURCEPP_BUILD_CSHARP_WRAPPERS)
        # Rely on C bindings
        set(SOURCEPP_BUILD_C_WRAPPERS ON CACHE INTERNAL "" FORCE)
    endif()
    set(SOURCEPP_BUILD_WASM_WRAPPERS OFF CACHE INTERNAL "" FORCE)
endif()

if(MSVC)
    # MSVC does not rely on tbb for std::execution policies, so we can force this on
    set(SOURCEPP_BUILD_WITH_TBB ON CACHE INTERNAL "" FORCE)
endif()


# Versioning published builds
if(PROJECT_IS_TOP_LEVEL AND NOT SOURCEPP_VERSION)
    set(SOURCEPP_VERSION "0.0.1")
    message(AUTHOR_WARNING "SOURCEPP_VERSION is not defined, do not release this build publicly! Defaulting it to ${SOURCEPP_VERSION}...")
endif()


# Set defaults after project call
if(PROJECT_IS_TOP_LEVEL)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)

    if(NOT SOURCEPP_BUILD_TESTS)
        include(CheckIPOSupported)
        check_ipo_supported(RESULT SOURCEPP_BUILD_WITH_LTO)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ${SOURCEPP_BUILD_WITH_LTO})
    endif()
endif()

if(SOURCEPP_USE_STATIC_MSVC_RUNTIME)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()


# Include thirdparty libraries
include_subdirectory(ext)


# Shared code
include_subdirectory(src/sourcepp)
include_subdirectory(src/sourcepp/compression)
include_subdirectory(src/sourcepp/crypto)
include_subdirectory(src/sourcepp/parser)


# Benchmarks
include_subdirectory(bench)
sourcepp_bench_setup()


# Bindings (pre)
include_subdirectory(lang)
sourcepp_lang_setup_pre()


# Tests (pre)
include_subdirectory(test)
sourcepp_test_setup_pre()


# Add libraries
add_sourcepp_library(bsppp             PYTHON                ) # sourcepp::bsppp
add_sourcepp_library(fspp                          TEST      ) # sourcepp::fspp
add_sourcepp_library(gamepp            PYTHON      TEST      ) # sourcepp::gamepp
add_sourcepp_library(kvpp              PYTHON      TEST BENCH) # sourcepp::kvpp
add_sourcepp_library(mdlpp                         TEST      ) # sourcepp::mdlpp
add_sourcepp_library(sndpp                         TEST      ) # sourcepp::sndpp
add_sourcepp_library(steampp  C        PYTHON      TEST      ) # sourcepp::steampp
add_sourcepp_library(toolpp            PYTHON      TEST      ) # sourcepp::toolpp
add_sourcepp_library(vcryptpp C        PYTHON      TEST      ) # sourcepp::vcryptpp
add_sourcepp_library(vpkpp             PYTHON      TEST      ) # sourcepp::vpkpp
add_sourcepp_library(vtfpp    C        PYTHON      TEST BENCH) # sourcepp::vtfpp


# Bindings (post)
sourcepp_lang_setup_post()


# Tests (post)
sourcepp_test_setup_post()


# Print options
print_options(OPTIONS
        USE_BSPPP USE_DMXPP USE_FSPP USE_GAMEPP USE_KVPP USE_MDLPP USE_SNDPP USE_STEAMPP USE_TOOLPP USE_VCRYPTPP USE_VPKPP USE_VTFPP "\n"
        BUILD_C_WRAPPERS BUILD_CSHARP_WRAPPERS BUILD_PYTHON_WRAPPERS BUILD_WASM_WRAPPERS "\n"
        BUILD_BENCHMARKS BUILD_WITH_TBB BUILD_WITH_THREADS BUILD_TESTS BUILD_TESTS_EXTRA BUILD_WIN7_COMPAT LINK_STATIC_MSVC_RUNTIME "\n")

if(SOURCEPP_USE_VPKPP)
    print_options(OPTIONS VPKPP_SUPPORT_VPK_V54 "\n")
endif()
if(SOURCEPP_USE_VTFPP)
    print_options(OPTIONS VTFPP_SUPPORT_EXR VTFPP_SUPPORT_QOI VTFPP_SUPPORT_WEBP "\n")
endif()
