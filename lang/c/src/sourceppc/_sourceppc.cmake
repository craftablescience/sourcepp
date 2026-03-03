add_pretty_parser(${PROJECT_NAME} C
        SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/lang/c/include/sourceppc/API.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/lang/c/include/sourceppc/Buffer.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/lang/c/include/sourceppc/Helpers.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/lang/c/include/sourceppc/String.h"
        "${CMAKE_CURRENT_LIST_DIR}/Buffer.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/String.cpp")

if(SOURCEPP_BUILD_CSHARP_WRAPPERS)
    add_dependencies(sourcepp_csharp sourcepp::${PROJECT_NAME}c)
    add_custom_command(TARGET sourcepp::${PROJECT_NAME}c POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/sourcepp_${PROJECT_NAME}c${CMAKE_SHARED_LIBRARY_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/lang/csharp/src")
endif()
