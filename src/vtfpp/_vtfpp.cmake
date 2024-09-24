add_pretty_parser(vtfpp
        DEPS miniz
        DEPS_INTERFACE sourcepp_stb
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

target_include_directories(vtfpp PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/ext/stb/include")

sourcepp_add_tbb(vtfpp)
sourcepp_add_threads(vtfpp)
target_link_compressonator(vtfpp)
