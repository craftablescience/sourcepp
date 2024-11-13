list(APPEND ${PROJECT_NAME}_crypto_HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/crypto/Adler32.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/crypto/CRC32.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/crypto/MD5.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/crypto/RSA.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/crypto/String.h")

add_library(${PROJECT_NAME}_crypto STATIC
        ${${PROJECT_NAME}_crypto_HEADERS}
        "${CMAKE_CURRENT_LIST_DIR}/Adler32.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/CRC32.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/MD5.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/RSA.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/String.cpp")

target_precompile_headers(${PROJECT_NAME}_crypto PUBLIC ${${PROJECT_NAME}_crypto_HEADERS})

target_link_libraries(${PROJECT_NAME}_crypto PUBLIC ${PROJECT_NAME} cryptopp::cryptopp)
