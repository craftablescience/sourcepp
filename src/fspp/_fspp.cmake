add_pretty_parser(fspp
        DEPS sourcepp::kvpp sourcepp::steampp
        DEPS_PUBLIC sourcepp::bsppp sourcepp::vpkpp
        SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/include/fspp/fspp.h"
        "${CMAKE_CURRENT_LIST_DIR}/fspp.cpp")
