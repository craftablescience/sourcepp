function(add_sourcepp_library TARGET)
    cmake_parse_arguments(PARSE_ARGV 1 OPTIONS "C;CSHARP;NO_TEST" "" "")
    string(TOUPPER ${TARGET} TARGET_UPPER)
    if(SOURCEPP_USE_${TARGET_UPPER})
        # Add C++
        include("${CMAKE_CURRENT_SOURCE_DIR}/src/${TARGET}/_${TARGET}.cmake")

        # Add C
        if(SOURCEPP_BUILD_C_WRAPPERS AND OPTIONS_C)
            include("${CMAKE_CURRENT_SOURCE_DIR}/lang/c/src/${TARGET}c/_${TARGET}c.cmake")
        endif()

        # Add C#
        if(OPTIONS_CSHARP)
            configure_file("${CMAKE_CURRENT_SOURCE_DIR}/lang/csharp/src/sourcepp/Buffer.cs.in"     "${CMAKE_CURRENT_SOURCE_DIR}/lang/csharp/src/${TARGET}/Buffer.cs")
            configure_file("${CMAKE_CURRENT_SOURCE_DIR}/lang/csharp/src/sourcepp/String.cs.in"     "${CMAKE_CURRENT_SOURCE_DIR}/lang/csharp/src/${TARGET}/String.cs")
            configure_file("${CMAKE_CURRENT_SOURCE_DIR}/lang/csharp/src/sourcepp/TARGET.csproj.in" "${CMAKE_CURRENT_SOURCE_DIR}/lang/csharp/src/${TARGET}/${TARGET}.csproj")
        endif()

        # Add tests
        if(NOT OPTIONS_NO_TEST AND SOURCEPP_BUILD_TESTS)
            list(APPEND ${SOURCEPP_TEST_NAME}_DEPS ${TARGET})
            list(APPEND ${SOURCEPP_TEST_NAME}_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/test/${TARGET}.cpp")
            return(PROPAGATE ${SOURCEPP_TEST_NAME}_DEPS ${SOURCEPP_TEST_NAME}_SOURCES)
        endif()
    endif()
endfunction()
