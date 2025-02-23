add_pretty_parser(vtfpp C
        PRECOMPILED_HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/lang/c/include/vtfppc/ImageConversion.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/lang/c/include/vtfppc/ImageFormats.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/lang/c/include/vtfppc/PPL.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/lang/c/include/vtfppc/SHT.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/lang/c/include/vtfppc/TTX.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/lang/c/include/vtfppc/VTF.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/lang/c/include/vtfppc/vtfpp.h"
        SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/lang/c/include/vtfppc/Convert.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/Convert.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/ImageConversion.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/ImageFormats.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/PPL.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/SHT.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/TTX.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/VTF.cpp")
