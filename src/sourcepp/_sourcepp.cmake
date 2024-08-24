list(APPEND ${PROJECT_NAME}_HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/crypto/Adler32.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/crypto/CRC32.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/crypto/MD5.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/crypto/RSA.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/crypto/String.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/math/Angles.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/math/Float.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/math/Integer.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/math/Matrix.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/math/Vector.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/parser/Binary.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/parser/Text.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/FS.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/Macros.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/String.h")

add_library(${PROJECT_NAME} STATIC
        ${${PROJECT_NAME}_HEADERS}
        "${CMAKE_CURRENT_LIST_DIR}/crypto/Adler32.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/crypto/CRC32.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/crypto/MD5.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/crypto/RSA.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/crypto/String.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/parser/Binary.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/parser/Text.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/FS.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/String.cpp")

target_precompile_headers(${PROJECT_NAME} PUBLIC ${${PROJECT_NAME}_HEADERS})

target_link_libraries(${PROJECT_NAME} PUBLIC bufferstream cryptopp::cryptopp)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
