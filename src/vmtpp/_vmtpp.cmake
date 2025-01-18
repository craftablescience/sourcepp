add_pretty_parser(vmtpp
        DEPS sourcepp::kvpp
        PRECOMPILED_HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vmtpp/EntityAccess.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vmtpp/Proxy.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vmtpp/VMT.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vmtpp/vmtpp.h"
        SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/EntityAccess.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Proxy.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/VMT.cpp")
