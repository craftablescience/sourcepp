cmake_minimum_required(VERSION 3.25 FATAL_ERROR)
project(sourcepp)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Options
option(SOURCEPP_USE_DMXPP               "Build dmxpp library"                       ON)
option(SOURCEPP_USE_FGDPP               "Build fgdpp library"                       ON)
option(SOURCEPP_USE_KVPP                "Build kvpp library"                        ON)
option(SOURCEPP_USE_MDLPP               "Build mdlpp library"                       ON)
option(SOURCEPP_USE_VICEPP              "Build vicepp library"                      ON)
option(SOURCEPP_USE_VPKPP               "Build vpkpp library"                       ON)
option(SOURCEPP_USE_VTFPP               "Build vtfpp library"                       ON)
option(SOURCEPP_BUILD_C_WRAPPERS        "Build C wrappers for supported libraries"  ON)
option(SOURCEPP_BUILD_TESTS             "Build tests for enabled libraries"        OFF)
option(SOURCEPP_USE_STATIC_MSVC_RUNTIME "Link to static MSVC runtime library"      OFF)


# Option overrides
if(SOURCEPP_USE_VPKPP)
    set(SOURCEPP_USE_KVPP ON CACHE INTERNAL "")
endif()


# Options per-library
option(FGDPP_ENABLE_SPEN_FGD_SUPPORT "Enable support for FGD alterations (https://github.com/TeamSpen210/HammerAddons/wiki/Unified-FGD) made by TeamSpen's HammerAddons. Fully backwards compatible with Valve's FGD standard." OFF)


# Set defaults
if(PROJECT_IS_TOP_LEVEL)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

if(SOURCEPP_USE_STATIC_MSVC_RUNTIME)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()


# bufferstream
if(NOT TARGET bufferstream)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/thirdparty/bufferstream")
endif()


# compressonator
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ext/compressonator")


# cryptopp
if (NOT TARGET cryptopp::cryptopp)
    set(CRYPTOPP_BUILD_TESTING OFF CACHE INTERNAL "")
    set(CRYPTOPP_INSTALL       OFF CACHE INTERNAL "")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/thirdparty/cryptopp")
endif()


# ice
if(SOURCEPP_USE_VICEPP)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ext/ice")
endif()


# miniz
if(SOURCEPP_USE_VTFPP AND NOT TARGET miniz)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/thirdparty/miniz")
endif()


# minizip-ng
if (SOURCEPP_USE_VPKPP AND NOT TARGET MINIZIP::minizip)
    set(MZ_COMPAT           OFF CACHE INTERNAL "")
    set(MZ_ZLIB             OFF CACHE INTERNAL "")
    set(MZ_BZIP2            OFF CACHE INTERNAL "")
    set(MZ_LZMA             OFF CACHE INTERNAL "")
    set(MZ_ZSTD             OFF CACHE INTERNAL "")
    set(MZ_LIBCOMP          OFF CACHE INTERNAL "")
    set(MZ_PKCRYPT          OFF CACHE INTERNAL "")
    set(MZ_WZAES            OFF CACHE INTERNAL "")
    set(MZ_OPENSSL          OFF CACHE INTERNAL "")
    set(MZ_FETCH_LIBS       ON  CACHE INTERNAL "")
    set(MZ_FORCE_FETCH_LIBS ON  CACHE INTERNAL "")
    set(SKIP_INSTALL_ALL    ON  CACHE INTERNAL "")
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/src/thirdparty/minizip-ng")
endif()


# stb
if(SOURCEPP_USE_VTFPP)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ext/stb")
endif()


# Shared code
add_library(${PROJECT_NAME} STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/buffer/Buffer.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/crypto/Adler32.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/crypto/CRC32.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/crypto/MD5.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/crypto/RSA.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/crypto/String.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/fs/FS.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/math/Angles.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/math/Float.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/math/Integer.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/math/Matrix.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/math/Vector.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/string/String.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/sourcepp/buffer/Buffer.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/sourcepp/crypto/Adler32.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/sourcepp/crypto/CRC32.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/sourcepp/crypto/MD5.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/sourcepp/crypto/RSA.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/sourcepp/crypto/String.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/sourcepp/fs/FS.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/sourcepp/string/String.cpp")
target_link_libraries(${PROJECT_NAME} PUBLIC bufferstream cryptopp::cryptopp)
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")


# Shared C code
if(SOURCEPP_BUILD_C_WRAPPERS)
    list(APPEND ${PROJECT_NAME}c_SOURCES
            "${CMAKE_CURRENT_SOURCE_DIR}/lang/c/include/sourceppc/API.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/lang/c/include/sourceppc/Buffer.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/lang/c/include/sourceppc/Convert.hpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/lang/c/include/sourceppc/Helpers.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/lang/c/include/sourceppc/String.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/lang/c/src/sourceppc/Buffer.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/lang/c/src/sourceppc/Convert.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/lang/c/src/sourceppc/String.cpp")
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


# Add scripts
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(AddPrettyParser)
include(AddSourcePPLibrary)


# Add libraries
add_sourcepp_library(dmxpp)
add_sourcepp_library(fgdpp)
add_sourcepp_library(kvpp)
add_sourcepp_library(mdlpp)
add_sourcepp_library(vicepp)
add_sourcepp_library(vpkpp C CSHARP)
add_sourcepp_library(vtfpp)


# Tests, part 2
if(SOURCEPP_BUILD_TESTS)
    add_executable(${SOURCEPP_TEST_NAME} ${${SOURCEPP_TEST_NAME}_SOURCES})
    target_link_libraries(${SOURCEPP_TEST_NAME} PUBLIC ${${SOURCEPP_TEST_NAME}_DEPS})
    target_compile_definitions(${SOURCEPP_TEST_NAME} PUBLIC ASSET_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/test/res/")

    include(GoogleTest)
    gtest_discover_tests(${SOURCEPP_TEST_NAME})
endif()
