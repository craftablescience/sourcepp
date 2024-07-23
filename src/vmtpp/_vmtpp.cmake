add_pretty_parser(vmtpp
        DEPS kvpp
        SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vmtpp/EntityAccess.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vmtpp/Proxy.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vmtpp/vmtpp.h"
        "${CMAKE_CURRENT_LIST_DIR}/EntityAccess.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Proxy.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/vmtpp.cpp")
