FetchContent_Declare(
        keyvalues
        GIT_REPOSITORY https://github.com/ozxybox/SpeedyKeyV.git
        GIT_TAG origin/master)
FetchContent_MakeAvailable(keyvalues)
target_optimize(keyvalues)

target_link_libraries(${TARGET}_bench PUBLIC keyvalues)
