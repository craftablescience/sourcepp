cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(sourcepp)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Options
option(USE_DMXPP         "Build dmxpp library"         ON)
option(USE_STUDIOMODELPP "Build studiomodelpp library" ON)


# BufferStream
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src/thirdparty/bufferstream")


# Shared code
list(APPEND ${PROJECT_NAME}_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/detail/Helpers.h"

        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/math/Matrix.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/math/Quaternion.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/math/Vector.h"

        "${CMAKE_CURRENT_SOURCE_DIR}/src/sourcepp/detail/Helpers.cpp")

add_library(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SOURCES})
target_link_libraries(${PROJECT_NAME} PUBLIC bufferstream)
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")


# dmxpp
if(USE_DMXPP)
    set(DMXPP_NAME "dmxpp")
    option(DMXPP_BUILD_TESTS "Build tests for dmxpp" OFF)

    list(APPEND ${DMXPP_NAME}_SOURCES
            "${CMAKE_CURRENT_SOURCE_DIR}/include/dmxpp/structs/Value.h"

            "${CMAKE_CURRENT_SOURCE_DIR}/include/dmxpp/dmxpp.h"

            "${CMAKE_CURRENT_SOURCE_DIR}/src/dmxpp/structs/Value.cpp"

            "${CMAKE_CURRENT_SOURCE_DIR}/src/dmxpp/dmxpp.cpp")

    add_library(${DMXPP_NAME} STATIC ${${DMXPP_NAME}_SOURCES})
    target_link_libraries(${DMXPP_NAME} PUBLIC ${PROJECT_NAME})
else()
    set(DMXPP_BUILD_TESTS OFF)
endif()


# studiomodelpp
if(USE_STUDIOMODELPP)
    set(STUDIOMODELPP_NAME "studiomodelpp")
    option(STUDIOMODELPP_BUILD_TESTS "Build tests for studiomodelpp" OFF)

    list(APPEND ${STUDIOMODELPP_NAME}_SOURCES
            "${CMAKE_CURRENT_SOURCE_DIR}/include/studiomodelpp/structs/MDL.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/include/studiomodelpp/structs/VTX.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/include/studiomodelpp/structs/VVD.h"

            "${CMAKE_CURRENT_SOURCE_DIR}/include/studiomodelpp/studiomodelpp.h"

            "${CMAKE_CURRENT_SOURCE_DIR}/src/studiomodelpp/structs/MDL.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/studiomodelpp/structs/VTX.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/studiomodelpp/structs/VVD.cpp"

            "${CMAKE_CURRENT_SOURCE_DIR}/src/studiomodelpp/studiomodelpp.cpp")

    add_library(${STUDIOMODELPP_NAME} STATIC ${${STUDIOMODELPP_NAME}_SOURCES})
    target_link_libraries(${STUDIOMODELPP_NAME} PUBLIC ${PROJECT_NAME})
else()
    set(STUDIOMODELPP_BUILD_TESTS OFF)
endif()


# Tests
if(DMXPP_BUILD_TESTS OR STUDIOMODELPP_BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.14.0)
    FetchContent_MakeAvailable(googletest)
    enable_testing()

    set(${PROJECT_NAME}_ASSET_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/test/res/")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/test/Helpers.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/test/Helpers.h")

    set(SOURCEPP_TEST_NAME "${PROJECT_NAME}_test")

    list(APPEND ${SOURCEPP_TEST_NAME}_SOURCES
            "${CMAKE_CURRENT_SOURCE_DIR}/test/Helpers.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/test/Helpers.cpp")
    if(DMXPP_BUILD_TESTS)
        list(APPEND ${SOURCEPP_TEST_NAME}_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/test/dmxpp.cpp")
    endif()
    if(STUDIOMODELPP_BUILD_TESTS)
        list(APPEND ${SOURCEPP_TEST_NAME}_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/test/studiomodelpp.cpp")
    endif()

    add_executable(${SOURCEPP_TEST_NAME} ${${SOURCEPP_TEST_NAME}_SOURCES})
    target_link_libraries(${SOURCEPP_TEST_NAME} PUBLIC gtest_main)
    if(DMXPP_BUILD_TESTS)
        target_link_libraries(${SOURCEPP_TEST_NAME} PUBLIC dmxpp)
    endif()
    if(STUDIOMODELPP_BUILD_TESTS)
        target_link_libraries(${SOURCEPP_TEST_NAME} PUBLIC studiomodelpp)
    endif()

    include(GoogleTest)
    gtest_discover_tests(${PROJECT_NAME}_test)
endif()
