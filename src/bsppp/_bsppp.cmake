add_pretty_parser(bsppp
        DEPS liblzma MINIZIP::minizip sourcepp_parser sourcepp::vpkpp
        PRECOMPILED_HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/include/bsppp/BSP.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/bsppp/bsppp.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/bsppp/EntityLump.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/bsppp/LumpData.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/bsppp/PakLump.h"
        SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/BSP.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/EntityLump.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/PakLump.cpp")

target_include_directories(bsppp PRIVATE "${xz_SOURCE_DIR}/src/liblzma/api")
