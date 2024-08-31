function(add_sourcepp_library TARGET)
    cmake_parse_arguments(PARSE_ARGV 1 OPTIONS "C;CSHARP;NO_TEST;BENCH" "" "")
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

        set(PROPAGATE_VARS "")

        # Add tests
        if(NOT OPTIONS_NO_TEST AND SOURCEPP_BUILD_TESTS)
            list(APPEND ${SOURCEPP_TEST_NAME}_DEPS ${TARGET})
            list(APPEND ${SOURCEPP_TEST_NAME}_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/test/${TARGET}.cpp")
            list(APPEND PROPAGATE_VARS ${SOURCEPP_TEST_NAME}_DEPS ${SOURCEPP_TEST_NAME}_SOURCES)
        endif()

        # Add benchmarks
        if(OPTIONS_BENCH AND SOURCEPP_BUILD_BENCHMARKS)
            include("${CMAKE_CURRENT_SOURCE_DIR}/test/bench/${TARGET}.cmake")
            list(APPEND ${SOURCEPP_BENCH_NAME}_DEPS ${TARGET})
            list(APPEND ${SOURCEPP_BENCH_NAME}_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/test/bench/${TARGET}.cpp")
            list(APPEND PROPAGATE_VARS ${SOURCEPP_BENCH_NAME}_DEPS ${SOURCEPP_BENCH_NAME}_SOURCES)
        endif()

        return(PROPAGATE ${PROPAGATE_VARS})
    endif()
endfunction()
