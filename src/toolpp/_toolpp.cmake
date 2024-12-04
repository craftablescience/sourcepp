add_pretty_parser(toolpp
        DEPS sourcepp_parser sourcepp::kvpp
        SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/include/toolpp/CmdSeq.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/toolpp/FGD.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/toolpp/toolpp.h"
        "${CMAKE_CURRENT_LIST_DIR}/CmdSeq.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/FGD.cpp")
