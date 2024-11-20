add_pretty_parser(mdlpp
        DEPS sourcepp_parser
        PRECOMPILED_HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/include/mdlpp/structs/Generic.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/mdlpp/structs/MDL.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/mdlpp/structs/VTX.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/mdlpp/structs/VVD.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/mdlpp/mdlpp.h"
        SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/structs/MDL.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/structs/VTX.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/structs/VVD.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/mdlpp.cpp")
