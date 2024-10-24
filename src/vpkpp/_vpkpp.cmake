add_pretty_parser(vpkpp
        DEPS cryptopp::cryptopp libzstd_static MINIZIP::minizip sourcepp::bsppp sourcepp::kvpp
        DEPS_PUBLIC tsl::hat_trie
        PRECOMPILED_HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vpkpp/format/BSP.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vpkpp/format/FPX.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vpkpp/format/GCF.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vpkpp/format/GMA.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vpkpp/format/PAK.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vpkpp/format/PCK.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vpkpp/format/VPK.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vpkpp/format/VPK_VTMB.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vpkpp/format/WAD3.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vpkpp/format/ZIP.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vpkpp/Attribute.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vpkpp/Entry.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vpkpp/Options.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vpkpp/PackFile.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vpkpp/PackFileType.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vpkpp/vpkpp.h"
        SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/format/BSP.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/format/FPX.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/format/GCF.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/format/GMA.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/format/PAK.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/format/PCK.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/format/VPK.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/format/VPK_VTMB.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/format/WAD3.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/format/ZIP.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/PackFile.cpp")

if(SOURCEPP_VPKPP_SUPPORT_VPK_V54)
    target_compile_definitions(vpkpp PRIVATE VPKPP_SUPPORT_VPK_V54)
endif()
