list(APPEND ${PROJECT_NAME}_compression_HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/compression/LZMA.h")

add_library(${PROJECT_NAME}_compression STATIC
        ${${PROJECT_NAME}_compression_HEADERS}
        "${CMAKE_CURRENT_LIST_DIR}/LZMA.cpp")

target_precompile_headers(${PROJECT_NAME}_compression PUBLIC ${${PROJECT_NAME}_compression_HEADERS})

target_link_libraries(${PROJECT_NAME}_compression PUBLIC ${PROJECT_NAME} ${PROJECT_NAME}_parser)
target_link_libraries(${PROJECT_NAME}_compression PRIVATE liblzma)

target_include_directories(${PROJECT_NAME}_compression PRIVATE "${xz_SOURCE_DIR}/src/liblzma/api")
