add_pretty_parser(bsppp
        DEPS MINIZIP::minizip sourcepp_compression sourcepp_parser
        DEPS_PUBLIC sourcepp::vpkpp
        PRECOMPILED_HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/include/bsppp/format/VBSP.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/bsppp/EntityLump.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/bsppp/PakLump.h"
        SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/format/VBSP.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/EntityLump.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/PakLump.cpp")
