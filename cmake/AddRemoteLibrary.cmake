include_guard(GLOBAL)

function(add_sourcepp_remote_library NAME REPOSITORY TAG)
    string(LENGTH "${TAG}" TAG_SIZE)
    if(NOT TAG_SIZE EQUAL 40)
        message(FATAL_ERROR "For the sake of making SourcePP builds reproducible, dependencies must be pinned to specific commits. ${NAME} is not.")
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
