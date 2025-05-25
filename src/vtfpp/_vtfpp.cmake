add_pretty_parser(vtfpp
        DEPS miniz libzstd_static sourcepp_compression sourcepp_parser sourcepp_qoi sourcepp_stb sourcepp_tinyexr
        PRECOMPILED_HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vtfpp/ImageConversion.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vtfpp/ImageFormats.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vtfpp/PPL.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vtfpp/SHT.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vtfpp/TTX.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vtfpp/VTF.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vtfpp/vtfpp.h"
        SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/ImageConversion.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/PPL.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/SHT.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/TTX.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/VTF.cpp")

sourcepp_add_opencl(sourcepp_vtfpp)
sourcepp_add_tbb(sourcepp_vtfpp)
sourcepp_add_threads(sourcepp_vtfpp)
target_link_compressonator(sourcepp_vtfpp)
