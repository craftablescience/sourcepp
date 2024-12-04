add_pretty_parser(kvpp
        DEPS sourcepp_parser
        SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/include/kvpp/KV1.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/kvpp/kvpp.h"
        "${CMAKE_CURRENT_LIST_DIR}/KV1.cpp")
