add_pretty_parser(vcdpp
        DEPS_PUBLIC sourcepp::vpkpp
        PRECOMPILED_HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vcdpp/IMAGE.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vcdpp/VCD.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vcdpp/vcdpp.h"
        SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/IMAGE.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/VCD.cpp")
