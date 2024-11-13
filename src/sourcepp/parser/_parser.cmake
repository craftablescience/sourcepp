list(APPEND ${PROJECT_NAME}_parser_HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/parser/Binary.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/parser/Text.h")

add_library(${PROJECT_NAME}_parser STATIC
        ${${PROJECT_NAME}_parser_HEADERS}
        "${CMAKE_CURRENT_LIST_DIR}/Binary.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Text.cpp")

target_precompile_headers(${PROJECT_NAME}_parser PUBLIC ${${PROJECT_NAME}_parser_HEADERS})

target_link_libraries(${PROJECT_NAME}_parser PUBLIC ${PROJECT_NAME})
