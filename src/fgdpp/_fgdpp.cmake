add_pretty_parser(fgdpp SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/include/fgdpp/structs/EntityProperties.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/fgdpp/fgdpp.h"
        "${CMAKE_CURRENT_LIST_DIR}/fgdpp.cpp")

if(FGDPP_ENABLE_SPEN_FGD_SUPPORT)
    target_compile_definitions(fgdpp PUBLIC FGDPP_UNIFIED_FGD)
endif()
