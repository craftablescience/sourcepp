add_pretty_parser(bsppp
        # BSP contains a packfile
        DEPS MINIZIP::minizip sourcepp::vpkpp
        SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/include/bsppp/bsppp.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/bsppp/BSPPack.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/bsppp/LumpData.h"
        "${CMAKE_CURRENT_LIST_DIR}/bsppp.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/BSPPack.cpp")
