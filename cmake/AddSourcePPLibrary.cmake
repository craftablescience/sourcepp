include_guard(GLOBAL)

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

            # Quick hack to let tell VS to place the dlls in the right spot
            # Can be removed/dropped if needed
            if(CMAKE_GENERATOR MATCHES "Visual Studio")
                add_custom_command(TARGET sourcepp::${TARGET}c POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/sourcepp_${TARGET}c${CMAKE_SHARED_LIBRARY_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/lang/csharp/src/${TARGET}")
            else()
                add_custom_command(TARGET sourcepp::${TARGET}c POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/sourcepp_${TARGET}c${CMAKE_SHARED_LIBRARY_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/lang/csharp/src/${TARGET}")
            endif()
        endif()

        # Add Python, WASM
        foreach(LANGUAGE "PYTHON" "WASM")
            if(SOURCEPP_BUILD_${LANGUAGE}_WRAPPERS AND OPTIONS_${LANGUAGE})
                string(TOLOWER "${LANGUAGE}" LANGUAGE_LOWER)
                list(APPEND ${${PROJECT_NAME}_${LANGUAGE}}_DEPS sourcepp::${TARGET})
                list(APPEND ${${PROJECT_NAME}_${LANGUAGE}}_DEFINES "SOURCEPP_${TARGET_UPPER}")
                list(APPEND ${${PROJECT_NAME}_${LANGUAGE}}_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/lang/${LANGUAGE_LOWER}/src/${TARGET}.h")
                list(APPEND PROPAGATE_VARS ${${PROJECT_NAME}_${LANGUAGE}}_DEPS ${${PROJECT_NAME}_${LANGUAGE}}_DEFINES ${${PROJECT_NAME}_${LANGUAGE}}_SOURCES)
            endif()
        endforeach()

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
