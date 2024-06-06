cmake_minimum_required(VERSION 3.16)
project(ice)
set(CMAKE_CXX_STANDARD 20)

# Create library
add_library(${PROJECT_NAME} STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include/IceKey.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/IceKey.cpp")

target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include")
