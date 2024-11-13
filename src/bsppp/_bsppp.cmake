add_pretty_parser(bsppp
        DEPS sourcepp_parser
        SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/include/bsppp/bsppp.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/bsppp/LumpData.h"
        "${CMAKE_CURRENT_LIST_DIR}/bsppp.cpp")
