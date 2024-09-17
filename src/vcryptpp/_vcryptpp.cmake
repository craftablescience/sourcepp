add_pretty_parser(vcryptpp
        DEPS sourcepp_ice
        SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vcryptpp/vcryptpp.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vcryptpp/VICE.h"
        "${CMAKE_CURRENT_LIST_DIR}/VICE.cpp")
