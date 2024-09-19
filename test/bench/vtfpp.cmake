FetchContent_Declare(
        vtflib
        GIT_REPOSITORY https://github.com/StrataSource/VTFLib.git
        GIT_TAG origin/master)
FetchContent_MakeAvailable(vtflib)
set_target_properties(vtflib PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

# I'm not distributing the benchmark program, so this should be fine
target_link_libraries(${TARGET}_bench PUBLIC vtflib_static)
