add_pretty_parser(bsppp
        DEPS MINIZIP::minizip sourcepp_compression sourcepp_parser
        DEPS_PUBLIC sourcepp::vpkpp
        PRECOMPILED_HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/include/bsppp/BSP.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/bsppp/bsppp.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/bsppp/EntityLump.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/bsppp/LumpData.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/bsppp/PakLump.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/bsppp/PakLumpXbox.h"
        SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/BSP.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/EntityLump.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/PakLump.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/PakLumpXbox.cpp")
