list(APPEND ${PROJECT_NAME}c_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/lang/c/include/sourceppc/API.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/lang/c/include/sourceppc/Buffer.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/lang/c/include/sourceppc/Convert.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/lang/c/include/sourceppc/Helpers.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/lang/c/include/sourceppc/String.h"
        "${CMAKE_CURRENT_LIST_DIR}/Buffer.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Convert.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/String.cpp")

if(TARGET ${PROJECT_NAME}_cwrap)
    target_compile_definitions(${PROJECT_NAME}_cwrap PRIVATE -DSOURCEPP_PATH="${CMAKE_CURRENT_SOURCE_DIR}/lang/c/include/sourceppc")
endif()
