function(add_sourcepp_library TARGET)
    cmake_parse_arguments(PARSE_ARGV 1 OPTIONS "C;CSHARP;PYTHON;WASM;TEST;BENCH" "" "")
    string(TOUPPER ${TARGET} TARGET_UPPER)
    if(SOURCEPP_USE_${TARGET_UPPER})
        set(PROPAGATE_VARS "")

        # Add C++
        include("${CMAKE_CURRENT_SOURCE_DIR}/src/${TARGET}/_${TARGET}.cmake")

        # Add C
        if(SOURCEPP_BUILD_C_WRAPPERS AND OPTIONS_C)
            include("${CMAKE_CURRENT_SOURCE_DIR}/lang/c/src/${TARGET}c/_${TARGET}c.cmake")
        endif()

        # Add C#
        if(SOURCEPP_BUILD_CSHARP_WRAPPERS AND OPTIONS_CSHARP)
            configure_file("${CMAKE_CURRENT_SOURCE_DIR}/lang/csharp/src/sourcepp/Buffer.cs.in"     "${CMAKE_CURRENT_SOURCE_DIR}/lang/csharp/src/${TARGET}/Buffer.cs")
            configure_file("${CMAKE_CURRENT_SOURCE_DIR}/lang/csharp/src/sourcepp/String.cs.in"     "${CMAKE_CURRENT_SOURCE_DIR}/lang/csharp/src/${TARGET}/String.cs")
            configure_file("${CMAKE_CURRENT_SOURCE_DIR}/lang/csharp/src/sourcepp/TARGET.csproj.in" "${CMAKE_CURRENT_SOURCE_DIR}/lang/csharp/src/${TARGET}/${TARGET}.csproj")
            add_custom_target(sourcepp_${TARGET}_csharp DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/lang/csharp/src/${TARGET}/${TARGET}.csproj")
            add_dependencies(sourcepp_${TARGET}_csharp sourcepp::${TARGET}c)
            add_custom_command(TARGET sourcepp::${TARGET}c POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/sourcepp_${TARGET}c${CMAKE_SHARED_LIBRARY_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/lang/csharp/src/${TARGET}")
        endif()

        # Add Python
        if(SOURCEPP_BUILD_PYTHON_WRAPPERS AND OPTIONS_PYTHON)
            list(APPEND ${${PROJECT_NAME}_PYTHON}_DEPS sourcepp::${TARGET})
            list(APPEND ${${PROJECT_NAME}_PYTHON}_DEFINES ${TARGET_UPPER})
            list(APPEND ${${PROJECT_NAME}_PYTHON}_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/lang/python/src/${TARGET}.h")
            list(APPEND PROPAGATE_VARS ${${PROJECT_NAME}_PYTHON}_DEPS ${${PROJECT_NAME}_PYTHON}_DEFINES ${${PROJECT_NAME}_PYTHON}_SOURCES)
        endif()

        # Add WASM
        if(SOURCEPP_BUILD_WASM_WRAPPERS AND OPTIONS_WASM)
            list(APPEND ${${PROJECT_NAME}_WASM}_DEPS sourcepp::${TARGET})
            list(APPEND ${${PROJECT_NAME}_WASM}_DEFINES ${TARGET_UPPER})
            list(APPEND ${${PROJECT_NAME}_WASM}_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/lang/wasm/src/${TARGET}.h")
            list(APPEND PROPAGATE_VARS ${${PROJECT_NAME}_WASM}_DEPS ${${PROJECT_NAME}_WASM}_DEFINES ${${PROJECT_NAME}_WASM}_SOURCES)
        endif()

        # Add tests
        if(SOURCEPP_BUILD_TESTS AND OPTIONS_TEST)
            list(APPEND ${SOURCEPP_TEST_NAME}_DEPS sourcepp::${TARGET})
            list(APPEND ${SOURCEPP_TEST_NAME}_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/test/${TARGET}.cpp")
            list(APPEND PROPAGATE_VARS ${SOURCEPP_TEST_NAME}_DEPS ${SOURCEPP_TEST_NAME}_SOURCES)
        endif()

        # Add benchmarks
        if(SOURCEPP_BUILD_BENCHMARKS AND OPTIONS_BENCH)
            add_executable(${TARGET}_bench "${CMAKE_CURRENT_SOURCE_DIR}/bench/${TARGET}.cpp")
            target_link_libraries(${TARGET}_bench PUBLIC ${SOURCEPP_BENCH_NAME} sourcepp::${TARGET})
            include("${CMAKE_CURRENT_SOURCE_DIR}/bench/${TARGET}.cmake")
        endif()

        return(PROPAGATE ${PROPAGATE_VARS})
    endif()
endfunction()
