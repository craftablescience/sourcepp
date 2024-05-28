cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(sourcepp)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Options (General)
option(SOURCEPP_USE_DMXPP         "Build dmxpp library"                ON)
option(SOURCEPP_USE_FGDPP         "Build fgdpp library"                ON)
option(SOURCEPP_USE_STUDIOMODELPP "Build studiomodelpp library"        ON)
option(SOURCEPP_USE_VMFPP         "Build vmfpp library"                ON)
option(SOURCEPP_USE_VTFPP         "Build vtfpp library"                ON)
option(SOURCEPP_BUILD_TESTS       "Build tests for enabled libraries" OFF)


# Options (Library)
option(FGDPP_ENABLE_SPEN_FGD_SUPPORT "Enable support for FGD alterations (https://github.com/TeamSpen210/HammerAddons/wiki/Unified-FGD) made by TeamSpen's HammerAddons. Fully backwards compatible with Valve's FGD standard." OFF)


# BufferStream
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/thirdparty/bufferstream")


# stb_image
if(SOURCEPP_USE_VTFPP)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/thirdparty/stb_image")
endif()


# Shared code
list(APPEND ${PROJECT_NAME}_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/detail/BufferUtils.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/detail/StringUtils.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/math/Angles.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/math/Integer.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/math/Matrix.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/math/Vector.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/sourcepp/detail/BufferUtils.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/sourcepp/detail/StringUtils.cpp")
add_library(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SOURCES})
target_link_libraries(${PROJECT_NAME} PUBLIC bufferstream)
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")


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

    set(${PROJECT_NAME}_ASSET_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/test/res/")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/test/Helpers.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/test/Helpers.h")

    list(APPEND ${SOURCEPP_TEST_NAME}_SOURCES
            "${CMAKE_CURRENT_SOURCE_DIR}/test/Helpers.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/test/Helpers.cpp")
    list(APPEND ${SOURCEPP_TEST_NAME}_DEPS gtest_main)
endif()


# Add scripts
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_scripts")
include(AddPrettyParser)


# dmxpp
if(SOURCEPP_USE_DMXPP)
    add_pretty_parser(dmxpp SOURCES
            "${CMAKE_CURRENT_SOURCE_DIR}/include/dmxpp/structs/Value.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/include/dmxpp/dmxpp.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/dmxpp/structs/Value.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/dmxpp/dmxpp.cpp")
endif()


# fgdpp
if(SOURCEPP_USE_FGDPP)
    add_pretty_parser(fgdpp SOURCES
            "${CMAKE_CURRENT_SOURCE_DIR}/include/fgdpp/structs/EntityProperties.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/include/fgdpp/fgdpp.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/fgdpp/fgdpp.cpp")
    if(FGDPP_ENABLE_SPEN_FGD_SUPPORT)
        target_compile_definitions(fgdpp PUBLIC FGDPP_UNIFIED_FGD)
    endif()
endif()


# studiomodelpp
if(SOURCEPP_USE_STUDIOMODELPP)
    add_pretty_parser(studiomodelpp SOURCES
            "${CMAKE_CURRENT_SOURCE_DIR}/include/studiomodelpp/structs/MDL.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/include/studiomodelpp/structs/VTX.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/include/studiomodelpp/structs/VVD.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/include/studiomodelpp/studiomodelpp.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/studiomodelpp/structs/MDL.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/studiomodelpp/structs/VTX.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/studiomodelpp/structs/VVD.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/studiomodelpp/studiomodelpp.cpp")
endif()


# vmfpp
if(SOURCEPP_USE_VMFPP)
    add_pretty_parser(vmfpp SOURCES
            "${CMAKE_CURRENT_SOURCE_DIR}/include/vmfpp/structs/Node.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/include/vmfpp/vmfpp.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/vmfpp/structs/Node.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/vmfpp/vmfpp.cpp")
endif()


# vtfpp
if(SOURCEPP_USE_VTFPP)
    add_pretty_parser(vtfpp SOURCES
            "${CMAKE_CURRENT_SOURCE_DIR}/include/vtfpp/ImageConversion.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/include/vtfpp/ImageFormats.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/include/vtfpp/vtfpp.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/vtfpp/ImageConversion.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/vtfpp/ImageFormats.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/vtfpp/vtfpp.cpp")
    target_link_libraries(vtfpp PRIVATE stb_image)
endif()


# Tests, part 2
if(SOURCEPP_BUILD_TESTS)
    add_executable(${SOURCEPP_TEST_NAME} ${${SOURCEPP_TEST_NAME}_SOURCES})
    target_link_libraries(${SOURCEPP_TEST_NAME} PUBLIC ${${SOURCEPP_TEST_NAME}_DEPS})
    include(GoogleTest)
    gtest_discover_tests(${SOURCEPP_TEST_NAME})
endif()
