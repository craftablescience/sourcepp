# bufferstream
add_sourcepp_remote_library(bufferstream https://github.com/craftablescience/BufferStream 070a5f0a510da1b982dd68f204ad154d8aaf55b1)


# compressonator
if(SOURCEPP_USE_VTFPP)
    add_sourcepp_remote_library(CMP_Compressonator https://github.com/craftablescience/compressonator a599efbe2e30f7c8e3287e15436ec6c1be580b99)
endif()


# cryptopp
if(NOT TARGET cryptopp::cryptopp)
    set(CRYPTOPP_BUILD_TESTING OFF CACHE INTERNAL "" FORCE)
    set(CRYPTOPP_INSTALL       OFF CACHE INTERNAL "" FORCE)
    add_sourcepp_remote_library(cryptopp-cmake https://github.com/abdes/cryptopp-cmake 866aceb8b13b6427a3c4541288ff412ad54f11ea)

    # hack: clang on windows (NOT clang-cl) needs these to compile cryptopp
    if(WIN32 AND NOT MSVC AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(cryptopp PRIVATE -mcrc32 -mssse3)
    endif()
endif()


# half
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/half")


# hat-trie
if(SOURCEPP_USE_VPKPP)
    add_sourcepp_remote_library(tsl_hat_trie https://github.com/Tessil/hat-trie 25fdf359711eb27e9e7ec0cfe19cc459ec6488d7)
endif()


# ice
if(SOURCEPP_USE_VCRYPTPP)
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/ice")
endif()


# miniz
if(SOURCEPP_USE_VPKPP OR SOURCEPP_USE_VTFPP)
    add_sourcepp_remote_library(miniz https://github.com/richgel999/miniz 174573d60290f447c13a2b1b3405de2b96e27d6c)
endif()


# minizip-ng
if(NOT TARGET MINIZIP::minizip)
    set(MZ_COMPAT           OFF CACHE INTERNAL "")
    set(MZ_LZMA             ON  CACHE INTERNAL "" FORCE)
    if(SOURCEPP_USE_VTFPP OR SOURCEPP_VPKPP_SUPPORT_VPK_V54)
        set(MZ_ZSTD         ON  CACHE INTERNAL "" FORCE)
    endif()
    set(MZ_FETCH_LIBS       ON  CACHE INTERNAL "" FORCE)
    set(MZ_FORCE_FETCH_LIBS ON  CACHE INTERNAL "" FORCE)
    set(MZ_PKCRYPT          OFF CACHE INTERNAL "")
    set(MZ_WZAES            OFF CACHE INTERNAL "")
    set(MZ_OPENSSL          OFF CACHE INTERNAL "")
    set(SKIP_INSTALL_ALL    ON  CACHE INTERNAL "" FORCE)
    add_sourcepp_remote_library(minizip-ng https://github.com/craftablescience/minizip-ng 860acee351f332387723472c7aca211c56eaeaa2)

    if(WIN32 AND SOURCEPP_BUILD_WIN7_COMPAT)
        set_source_files_properties(
                "${CMAKE_CURRENT_LIST_DIR}/minizip-ng/mz_os_win32.c"
                "${CMAKE_CURRENT_LIST_DIR}/minizip-ng/mz_strm_os_win32.c"
                DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/minizip-ng"
                PROPERTIES COMPILE_DEFINITIONS _WIN32_WINNT=0x0601)
    endif()
endif()


# qoi
if(SOURCEPP_USE_VTFPP AND SOURCEPP_VTFPP_SUPPORT_QOI)
    if(NOT TARGET qoi)
        add_sourcepp_remote_library(qoi https://github.com/phoboslab/qoi 316593b6de3576743506d4115e30bf03a12b587d)
        add_library(qoi INTERFACE "${qoi_SOURCE_DIR}/qoi.h")
        target_include_directories(qoi INTERFACE "$<BUILD_INTERFACE:${qoi_SOURCE_DIR}>" "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
    endif()
endif()


# stb
if(SOURCEPP_USE_VTFPP)
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/stb")
endif()


# tinyexr
if(SOURCEPP_USE_VTFPP AND SOURCEPP_VTFPP_SUPPORT_EXR)
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/tinyexr")
endif()


# tbb
function(sourcepp_add_tbb TARGET)
    if(SOURCEPP_BUILD_WITH_TBB)
        target_compile_definitions(${TARGET} PUBLIC SOURCEPP_BUILD_WITH_TBB)
        if(NOT MSVC)
            target_link_libraries(${TARGET} PRIVATE tbb)
        endif()
    endif()
endfunction()


# Threads
if(SOURCEPP_BUILD_WITH_THREADS)
    set(CMAKE_THREAD_PREFER_PTHREAD ON)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads)
    if(NOT Threads_FOUND)
        set(SOURCEPP_BUILD_WITH_THREADS OFF CACHE INTERNAL "" FORCE)
    endif()
endif()

function(sourcepp_add_threads TARGET)
    if(SOURCEPP_BUILD_WITH_THREADS)
        target_compile_definitions(${TARGET} PUBLIC SOURCEPP_BUILD_WITH_THREADS)
        target_link_libraries(${TARGET} PRIVATE Threads::Threads)
    endif()
endfunction()


# webp
if(SOURCEPP_USE_VTFPP AND SOURCEPP_VTFPP_SUPPORT_WEBP)
    set(WEBP_BUILD_ANIM_UTILS                      OFF CACHE INTERNAL "" FORCE)
    set(WEBP_BUILD_CWEBP                           OFF CACHE INTERNAL "" FORCE)
    set(WEBP_BUILD_DWEBP                           OFF CACHE INTERNAL "" FORCE)
    set(WEBP_BUILD_GIF2WEBP                        OFF CACHE INTERNAL "" FORCE)
    set(WEBP_BUILD_IMG2WEBP                        OFF CACHE INTERNAL "" FORCE)
    set(WEBP_BUILD_VWEBP                           OFF CACHE INTERNAL "" FORCE)
    set(WEBP_BUILD_WEBPINFO                        OFF CACHE INTERNAL "" FORCE)
    set(WEBP_BUILD_LIBWEBPMUX                      OFF CACHE INTERNAL "" FORCE)
    set(WEBP_BUILD_WEBPMUX                         OFF CACHE INTERNAL "" FORCE)
    set(WEBP_BUILD_EXTRAS                          OFF CACHE INTERNAL "" FORCE)
    set(WEBP_BUILD_WEBP_JS                         OFF CACHE INTERNAL "" FORCE)
    set(WEBP_BUILD_FUZZTEST                        OFF CACHE INTERNAL "" FORCE)
    set(WEBP_USE_THREAD ${SOURCEPP_BUILD_WITH_THREADS} CACHE INTERNAL "" FORCE)
    set(WEBP_NEAR_LOSSLESS                          ON CACHE INTERNAL "" FORCE)
    add_sourcepp_remote_library(webp https://github.com/webmproject/libwebp 74f6afd3e6ea8ff5b231ad2248b52bdcd1666c70)
endif()
