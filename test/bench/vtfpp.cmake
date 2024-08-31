FetchContent_Declare(
        vtflib
        GIT_REPOSITORY https://github.com/StrataSource/VTFLib.git
        GIT_TAG origin/master)
FetchContent_MakeAvailable(vtflib)
set_target_properties(vtflib PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

list(APPEND ${SOURCEPP_BENCH_NAME}_DEPS vtflib)
