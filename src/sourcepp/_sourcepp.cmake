add_library(${PROJECT_NAME} STATIC
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
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/parser/Binary.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/parser/Text.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/string/String.h"
        "${CMAKE_CURRENT_LIST_DIR}/crypto/Adler32.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/crypto/CRC32.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/crypto/MD5.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/crypto/RSA.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/crypto/String.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/fs/FS.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/parser/Binary.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/parser/Text.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/string/String.cpp")

target_sources(${PROJECT_NAME} PUBLIC FILE_SET CXX_MODULES FILES "${CMAKE_CURRENT_LIST_DIR}/sourcepp.cppm")

target_link_libraries(${PROJECT_NAME} PUBLIC bufferstream cryptopp::cryptopp)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
