include_guard(GLOBAL)

include(FetchContent)

function(add_sourcepp_remote_library NAME REPOSITORY TAG)
    cmake_parse_arguments(PARSE_ARGV 3 OPTIONS "ALLOW_NOT_PINNED" "" "")
    if(NOT OPTIONS_ALLOW_NOT_PINNED)
        string(LENGTH "${TAG}" TAG_SIZE)
        if(NOT TAG_SIZE EQUAL 40)
            message(FATAL_ERROR "For the sake of making SourcePP builds reproducible, dependencies must be pinned to specific commits. ${NAME} is not.")
        endif()
    endif()
    if(NOT TARGET NAME)
        message(STATUS "Fetching ${NAME} ${REPOSITORY} ${TAG}")
        string(REPLACE ":" "_" NAME_CLEAN "${NAME}")
        FetchContent_Declare(${NAME_CLEAN}
                GIT_REPOSITORY "${REPOSITORY}.git"
                GIT_TAG        "${TAG}")
        FetchContent_MakeAvailable(${NAME_CLEAN})
    endif()
    return(PROPAGATE "${NAME_CLEAN}_POPULATED" "${NAME_CLEAN}_SOURCE_DIR" "${NAME_CLEAN}_BINARY_DIR")
endfunction()
