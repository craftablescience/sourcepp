add_pretty_parser(vtfpp
        DEPS miniz sourcepp_stb
        SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vtfpp/ImageConversion.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vtfpp/ImageFormats.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vtfpp/vtfpp.h"
        "${CMAKE_CURRENT_LIST_DIR}/ImageConversion.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/vtfpp.cpp"
        MODULES
        "${CMAKE_CURRENT_LIST_DIR}/vtfpp.cppm")

target_link_compressonator(vtfpp)
