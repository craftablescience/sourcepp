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
    add_sourcepp_remote_library(bcdec https://github.com/craftablescience/bcdec 59441e17ba36b7d7eef336aeedc62e01d0cdcd5a)
endif()


# bufferstream
add_sourcepp_remote_library(bufferstream https://github.com/craftablescience/BufferStream 2a7c9e8b786fa50a3ae1961f1ee5bca6c4a5a6c5)


# compressonator
if(SOURCEPP_USE_VTFPP AND SOURCEPP_VTFPP_BUILD_WITH_COMPRESSONATOR)
    add_sourcepp_remote_library(CMP_Compressonator https://github.com/craftablescience/compressonator f9c8c58fe753108c260b33ec32301805c33c08b7)
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


# libtommath
if(NOT TARGET libtommath)
    add_sourcepp_remote_library(libtommath https://github.com/libtom/libtommath ae40a87a920099a7d9d00979570e0c8d917a1fd7 OVERRIDE_FIND_PACKAGE)

    # hack: always compile with optimizations on MSVC because this library stinks
    if(WIN32 AND MSVC)
        get_target_property(libtommath_CURRENT_OPTIONS libtommath COMPILE_OPTIONS)
        if(libtommath_CURRENT_OPTIONS)
            list(REMOVE_ITEM libtommath_CURRENT_OPTIONS "/RTC1")
            list(APPEND libtommath_CURRENT_OPTIONS "/O2")
            set_target_properties(libtommath PROPERTIES COMPILE_OPTIONS "${libtommath_CURRENT_OPTIONS}")
        endif()
    endif()
endif()


# libtomcrypt
if(NOT TARGET libtomcrypt)
    if(SOURCEPP_BUILD_WITH_THREADS AND CMAKE_USE_PTHREADS_INIT)
        set(WITH_PTHREAD ON CACHE INTERNAL "" FORCE)
    endif()
    add_sourcepp_remote_library(libtomcrypt https://github.com/libtom/libtomcrypt c80285ba04f87ee5359baf689ccc7ce8a31116dc)
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
    add_sourcepp_remote_library(libwebp https://github.com/webmproject/libwebp 5003e5609eedc5680b8d838a962cbb9a6e9709ce EXCLUDE_FROM_ALL)
endif()


# miniz
if(SOURCEPP_USE_VPKPP OR SOURCEPP_USE_VTFPP)
    add_sourcepp_remote_library(miniz https://github.com/richgel999/miniz 5cf1e56a9c968c11fdd1a6414f3a95f84314c437)
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

    set(ZSTD_MULTITHREAD_SUPPORT ${SOURCEPP_BUILD_WITH_THREADS} CACHE INTERNAL "" FORCE)

    add_sourcepp_remote_library(minizip-ng https://github.com/craftablescience/minizip-ng de1f8bba0b7dbd0920289768edad8d878c95421f)

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
        add_sourcepp_remote_library(qoi https://github.com/phoboslab/qoi e084ec009b38c755acc40fe31d3f83ee17935b9d)
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
        add_sourcepp_remote_library(tinyexr https://github.com/syoyo/tinyexr 4946b5d92e13bcc8102ac2c8efd129596a90bf75 DO_NOT_USE_CMAKELISTS)
        add_library(tinyexr STATIC "${tinyexr_SOURCE_DIR}/exr_reader.hh" "${tinyexr_SOURCE_DIR}/streamreader.hh" "${tinyexr_SOURCE_DIR}/streamwriter.hh" "${tinyexr_SOURCE_DIR}/tinyexr.cc" "${tinyexr_SOURCE_DIR}/tinyexr.h")
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
