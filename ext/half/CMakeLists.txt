cmake_minimum_required(VERSION 3.16)
project(sourcepp_half)
set(CMAKE_CXX_STANDARD 20)

# Create library
add_library(${PROJECT_NAME} INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/include/half.hpp")

target_include_directories(${PROJECT_NAME} INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

if(EMSCRIPTEN)
    target_compile_definitions(${PROJECT_NAME} INTERFACE
            HALF_ENABLE_CPP11_CFENV=0)
endif()
