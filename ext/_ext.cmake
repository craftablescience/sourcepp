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


# bcdec
if(SOURCEPP_USE_VTFPP)
    add_sourcepp_remote_library(bcdec https://github.com/craftablescience/bcdec c3542bb56443d2b1f596127200b062b6ee2888c2)
endif()


# bufferstream
add_sourcepp_remote_library(bufferstream https://github.com/craftablescience/BufferStream 5c040a17417a3cef650f49c41192b2c00ac1e865)


# compressonator
if(SOURCEPP_USE_VTFPP AND SOURCEPP_VTFPP_BUILD_WITH_COMPRESSONATOR)
    add_sourcepp_remote_library(CMP_Compressonator https://github.com/craftablescience/compressonator-clean deb05dff448817b4b7a35f34fc293d6ebd913951)
endif()


# half
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/half")


# hat-trie
if(SOURCEPP_USE_VPKPP)
    add_sourcepp_remote_library(tsl_hat_trie https://github.com/Tessil/hat-trie f1380d704eccf753db5f9df175789fff8ff353e0)
endif()


# ice
if(SOURCEPP_USE_VCRYPTPP)
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/ice")
endif()


# libjxl
if(SOURCEPP_USE_VTFPP AND SOURCEPP_VTFPP_SUPPORT_JXL)
    set(JPEGXL_ENABLE_TOOLS                                   OFF CACHE INTERNAL "" FORCE)
    set(JPEGXL_ENABLE_DOXYGEN                                 OFF CACHE INTERNAL "" FORCE)
    set(JPEGXL_ENABLE_MANPAGES                                OFF CACHE INTERNAL "" FORCE)
    set(JPEGXL_ENABLE_BENCHMARK                               OFF CACHE INTERNAL "" FORCE)
    set(JPEGXL_ENABLE_EXAMPLES                                OFF CACHE INTERNAL "" FORCE)
    set(JPEGXL_ENABLE_JNI                                     OFF CACHE INTERNAL "" FORCE)
    set(JPEGXL_ENABLE_OPENEXR                                 OFF CACHE INTERNAL "" FORCE)
    set(JPEGXL_ENABLE_WASM_THREADS ${SOURCEPP_BUILD_WITH_THREADS} CACHE INTERNAL "" FORCE)
    add_sourcepp_remote_library(libjxl https://github.com/libjxl/libjxl b5def9fb509d0f2421c8a5bcd7aa6f5a627363c4 EXCLUDE_FROM_ALL)
endif()


# libtommath
if(NOT TARGET libtommath)
    add_sourcepp_remote_library(libtommath https://github.com/craftablescience/libtommath 03101d4556acd45175d5cfe0575601cf6acadef2 OVERRIDE_FIND_PACKAGE EXCLUDE_FROM_ALL)
endif()


# libtomcrypt
if(NOT TARGET libtomcrypt)
    if(SOURCEPP_BUILD_WITH_THREADS AND CMAKE_USE_PTHREADS_INIT)
        set(WITH_PTHREAD ON CACHE INTERNAL "" FORCE)
    endif()
    add_sourcepp_remote_library(libtomcrypt https://github.com/libtom/libtomcrypt 6c6d5104de66f3ca0dfd7b68540ef86869982b07 EXCLUDE_FROM_ALL)
    if(MSVC)
        # Spews "inconsistent dll linkage", no idea how to fix, doesn't seem to cause problems
        target_compile_options(libtomcrypt PRIVATE "/wd4273")
    endif()
endif()


# libwebp
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
    add_sourcepp_remote_library(libwebp https://github.com/webmproject/libwebp 2549f353596b9990b5eeccbe03c898932ac8ab5c EXCLUDE_FROM_ALL)
endif()


# miniz
if(SOURCEPP_USE_VPKPP OR SOURCEPP_USE_VTFPP)
    add_sourcepp_remote_library(miniz https://github.com/richgel999/miniz 77d0dce8627735138c51770d1799a1ef48f2117d)
endif()


# minizip-ng
if(NOT TARGET MINIZIP::minizip)
    set(MZ_COMPAT           OFF CACHE INTERNAL "")
    set(MZ_LZMA              ON CACHE INTERNAL "" FORCE)
    if(SOURCEPP_USE_VTFPP OR SOURCEPP_VPKPP_SUPPORT_VPK_V54)
        set(MZ_ZSTD          ON CACHE INTERNAL "" FORCE)
    endif()
    set(MZ_FETCH_LIBS        ON CACHE INTERNAL "" FORCE)
    set(MZ_FORCE_FETCH_LIBS  ON CACHE INTERNAL "" FORCE)
    set(MZ_PKCRYPT          OFF CACHE INTERNAL "")
    set(MZ_WZAES            OFF CACHE INTERNAL "")
    set(MZ_OPENSSL          OFF CACHE INTERNAL "")
    set(SKIP_INSTALL_ALL     ON CACHE INTERNAL "" FORCE)

    set(ZSTD_MULTITHREAD_SUPPORT ${SOURCEPP_BUILD_WITH_THREADS} CACHE INTERNAL "" FORCE)

    add_sourcepp_remote_library(minizip-ng https://github.com/craftablescience/minizip-ng 2f0041b6f7c2193a06d18ca47ccd81fc7070ee8f)

    if(WIN32 AND SOURCEPP_BUILD_WIN7_COMPAT)
        set_source_files_properties(
                "${minizip-ng_SOURCE_DIR}/mz_os_win32.c"
                "${minizip-ng_SOURCE_DIR}/mz_strm_os_win32.c"
                DIRECTORY "${minizip-ng_SOURCE_DIR}"
                PROPERTIES COMPILE_DEFINITIONS _WIN32_WINNT=0x0601)
    endif()
endif()


# qoi
if(SOURCEPP_USE_VTFPP AND SOURCEPP_VTFPP_SUPPORT_QOI)
    if(NOT TARGET qoi)
        add_sourcepp_remote_library(qoi https://github.com/phoboslab/qoi 97bacc86a9c4abf5a2d452102dc26546c4c670b9)
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
    if(NOT TARGET tinyexr)
        add_sourcepp_remote_library(tinyexr https://github.com/syoyo/tinyexr 644148d0fd6b1b204a68a902dc963a70c749b417 DO_NOT_USE_CMAKELISTS)
        add_library(tinyexr STATIC "${tinyexr_SOURCE_DIR}/exr_reader.hh" "${tinyexr_SOURCE_DIR}/streamreader.hh" "${tinyexr_SOURCE_DIR}/tinyexr.cc" "${tinyexr_SOURCE_DIR}/tinyexr.h")
        target_include_directories(tinyexr PUBLIC "${tinyexr_SOURCE_DIR}")
        target_link_libraries(tinyexr PUBLIC miniz)
        if(SOURCEPP_BUILD_WITH_THREADS)
            target_compile_definitions(tinyexr PUBLIC TINYEXR_USE_THREAD=1)
        endif()
    endif()
endif()


# tbb
function(sourcepp_add_tbb TARGET)
    if(SOURCEPP_BUILD_WITH_TBB)
        target_compile_definitions(${TARGET} PUBLIC SOURCEPP_BUILD_WITH_TBB)
        if(NOT MSVC)
            target_link_libraries(${TARGET} PUBLIC tbb)
        endif()
    endif()
endfunction()
