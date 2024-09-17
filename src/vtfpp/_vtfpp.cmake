add_pretty_parser(vtfpp
        DEPS miniz sourcepp_stb
        PRECOMPILED_HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vtfpp/ImageConversion.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vtfpp/ImageFormats.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vtfpp/PPL.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vtfpp/VTF.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vtfpp/vtfpp.h"
        SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/ImageConversion.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/PPL.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/VTF.cpp")

sourcepp_add_tbb(vtfpp)
target_link_compressonator(vtfpp)
