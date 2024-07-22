add_pretty_parser(steampp
        DEPS kvpp
        SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/include/steampp/steampp.h"
        "${CMAKE_CURRENT_LIST_DIR}/steampp.cpp"
        MODULES
        "${CMAKE_CURRENT_LIST_DIR}/steampp.cppm")
