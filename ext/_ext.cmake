# bufferstream
if(NOT TARGET bufferstream)
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/bufferstream")
endif()


# compressonator
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/compressonator")


# cryptopp
if(NOT TARGET cryptopp::cryptopp)
    set(CRYPTOPP_BUILD_TESTING OFF CACHE INTERNAL "" FORCE)
    set(CRYPTOPP_INSTALL       OFF CACHE INTERNAL "" FORCE)
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/cryptopp")

    # hack: clang on windows (NOT clang-cl) needs these to compile cryptopp
    if(WIN32 AND NOT MSVC AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(cryptopp PRIVATE -mcrc32 -mssse3)
    endif()
endif()


# half
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/half")


# hat-trie
if(NOT TARGET tsl::hat_trie)
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/hat-trie")
endif()


# ice
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/ice")


# miniz
if(NOT TARGET miniz)
    set(BUILD_NO_STDIO ON CACHE INTERNAL "" FORCE)
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/miniz")
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
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/minizip-ng")

    if(WIN32 AND SOURCEPP_BUILD_WIN7_COMPAT)
        set_source_files_properties(
                "${CMAKE_CURRENT_LIST_DIR}/minizip-ng/mz_os_win32.c"
                "${CMAKE_CURRENT_LIST_DIR}/minizip-ng/mz_strm_os_win32.c"
                DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/minizip-ng"
                PROPERTIES COMPILE_DEFINITIONS _WIN32_WINNT=0x0601)
    endif()
endif()


# qoi
add_library(sourcepp_qoi INTERFACE "${CMAKE_CURRENT_LIST_DIR}/qoi/qoi.h")
target_include_directories(sourcepp_qoi INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/qoi>" "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")


# stb
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/stb")


# tinyexr
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/tinyexr")


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
if(NOT TARGET webp)
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
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/libwebp" EXCLUDE_FROM_ALL)
endif()
