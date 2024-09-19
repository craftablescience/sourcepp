FetchContent_Declare(
        vtflib
        GIT_REPOSITORY https://github.com/StrataSource/VTFLib.git
        GIT_TAG origin/master)
FetchContent_MakeAvailable(vtflib)
target_optimize(vtflib_static)

# I'm not distributing the benchmark program, so this should be fine
target_link_libraries(${TARGET}_bench PUBLIC vtflib_static)
