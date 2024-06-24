function(add_sourcepp_library TARGET)
    cmake_parse_arguments(PARSE_ARGV 1 OPTIONS "C;CSHARP" "" "")
    string(TOUPPER ${TARGET} TARGET_UPPER)
    if(SOURCEPP_USE_${TARGET_UPPER})
        include("${CMAKE_CURRENT_SOURCE_DIR}/src/${TARGET}/_${TARGET}.cmake")
        if(SOURCEPP_BUILD_C_WRAPPERS AND OPTIONS_C)
            include("${CMAKE_CURRENT_SOURCE_DIR}/lang/c/src/${TARGET}c/_${TARGET}c.cmake")
        endif()
        if(OPTIONS_CSHARP)
            configure_file(
                    "${CMAKE_CURRENT_SOURCE_DIR}/lang/csharp/${TARGET}/${TARGET}.csproj.in"
                    "${CMAKE_CURRENT_SOURCE_DIR}/lang/csharp/${TARGET}/${TARGET}.csproj")
        endif()

        # Gross hack
        set(${SOURCEPP_TEST_NAME}_SOURCES ${${SOURCEPP_TEST_NAME}_SOURCES} PARENT_SCOPE)
        set(${SOURCEPP_TEST_NAME}_DEPS ${${SOURCEPP_TEST_NAME}_DEPS} PARENT_SCOPE)
    endif()
endfunction()
