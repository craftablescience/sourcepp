# Add a new parser library
function(add_pretty_parser TARGET)
    cmake_parse_arguments(PARSE_ARGV 1 OPTIONS "C" "" "DEPS;SOURCES")

    if(OPTIONS_C)
        add_library(${TARGET}c SHARED ${${PROJECT_NAME}c_SOURCES} ${OPTIONS_SOURCES})
        set_target_properties(${TARGET}c PROPERTIES PREFIX "")
        target_link_libraries(${TARGET}c PRIVATE ${TARGET})
        target_include_directories(${TARGET}c PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/lang/c/include")
        set(TARGET "${TARGET}c")
    else()
        add_library(${TARGET} STATIC ${OPTIONS_SOURCES})
    endif()
    target_link_libraries(${TARGET} PUBLIC ${PROJECT_NAME})
    target_link_libraries(${TARGET} PRIVATE ${OPTIONS_DEPS})

    # Define DEBUG macro
    target_compile_definitions(${TARGET} PRIVATE "$<$<CONFIG:Debug>:DEBUG>")

    # Set optimization flags
    if(CMAKE_BUILD_TYPE MATCHES "Debug")
        # Build with debug friendly optimizations and debug symbols (MSVC defaults are fine)
        if(UNIX OR MINGW)
            target_compile_options(${TARGET} PRIVATE -Og -g)
        endif()
        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND NOT MSVC)
            target_compile_options(${TARGET} PRIVATE -fno-limit-debug-info)
        endif()
    else()
        # Build with optimizations and don't omit stack pointer for debugging (MSVC defaults are fine)
        if(UNIX OR MINGW)
            target_compile_options(${TARGET} PRIVATE -O2 -fno-omit-frame-pointer)
        endif()
    endif()

    # Add tests
    if(NOT OPTIONS_C AND SOURCEPP_BUILD_TESTS)
        list(APPEND ${SOURCEPP_TEST_NAME}_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/test/${TARGET}.cpp")
        list(APPEND ${SOURCEPP_TEST_NAME}_DEPS ${TARGET})

        # Gross hack
        set(${SOURCEPP_TEST_NAME}_SOURCES ${${SOURCEPP_TEST_NAME}_SOURCES} PARENT_SCOPE)
        set(${SOURCEPP_TEST_NAME}_DEPS ${${SOURCEPP_TEST_NAME}_DEPS} PARENT_SCOPE)
    endif()
endfunction()
