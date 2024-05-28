cmake_minimum_required(VERSION 3.16)
project(stb_image)
set(CMAKE_CXX_STANDARD 20)

# Create library
add_library(${PROJECT_NAME} STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/stb_image.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/stb_image_write.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/stb_impl.cpp")

target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}")
