# Add a new parser library
function(add_pretty_parser NAME)
    cmake_parse_arguments(PARSE_ARGV 1 OPTIONS "" "" "DEPS;SOURCES")
    add_library(${NAME} STATIC ${OPTIONS_SOURCES})
    target_link_libraries(${NAME} PUBLIC ${PROJECT_NAME} ${OPTIONS_DEPS})

    if(SOURCEPP_BUILD_TESTS)
        list(APPEND ${SOURCEPP_TEST_NAME}_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/test/${NAME}.cpp")
        list(APPEND ${SOURCEPP_TEST_NAME}_DEPS ${NAME})
        # Yuck
        set(${SOURCEPP_TEST_NAME}_SOURCES ${${SOURCEPP_TEST_NAME}_SOURCES} PARENT_SCOPE)
        set(${SOURCEPP_TEST_NAME}_DEPS ${${SOURCEPP_TEST_NAME}_DEPS} PARENT_SCOPE)
    endif()
endfunction()
