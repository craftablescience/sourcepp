# Add a new parser library
function(add_pretty_parser TARGET)
    cmake_parse_arguments(PARSE_ARGV 1 OPTIONS "C" "" "DEPS;DEPS_INTERFACE;PRECOMPILED_HEADERS;SOURCES")

    if(OPTIONS_C)
        add_library(${TARGET}c SHARED ${${PROJECT_NAME}c_SOURCES} ${OPTIONS_PRECOMPILED_HEADERS} ${OPTIONS_SOURCES})
        add_library(sourcepp::${TARGET}c ALIAS ${TARGET}c)
        set_target_properties(${TARGET}c PROPERTIES PREFIX "")
        target_link_libraries(${TARGET}c PRIVATE ${TARGET})
        target_include_directories(${TARGET}c PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/lang/c/include")
        set(TARGET "${TARGET}c")
    else()
        add_library(${TARGET} STATIC ${OPTIONS_PRECOMPILED_HEADERS} ${OPTIONS_SOURCES})
        add_library(sourcepp::${TARGET} ALIAS ${TARGET})
    endif()
    if(NOT ("PRECOMPILED_HEADERS" IN_LIST OPTIONS_UNPARSED_ARGUMENTS))
        target_precompile_headers(${TARGET} PUBLIC ${OPTIONS_HEADERS})
    endif()
    target_link_libraries(${TARGET} PUBLIC ${PROJECT_NAME})
    target_link_libraries(${TARGET} PRIVATE ${OPTIONS_DEPS})
    target_link_libraries(${TARGET} INTERFACE ${OPTIONS_DEPS_INTERFACE})

    # Define DEBUG macro
    target_compile_definitions(${TARGET} PRIVATE "$<$<CONFIG:Debug>:DEBUG>")

    # MSVC on its bullshit again
    if(MSVC)
        target_compile_options(${TARGET} PRIVATE "/Zc:preprocessor")
    endif()

    target_optimize(${TARGET})
endfunction()
