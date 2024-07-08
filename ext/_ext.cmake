# bufferstream
if(NOT TARGET bufferstream)
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/bufferstream")
endif()


# compressonator
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/compressonator")


# cryptopp
if(NOT TARGET cryptopp::cryptopp)
    set(CRYPTOPP_BUILD_TESTING OFF CACHE INTERNAL "")
    set(CRYPTOPP_INSTALL       OFF CACHE INTERNAL "")
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/cryptopp")
endif()


# gdeflate
if(NOT TARGET gdeflate)
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/gdeflate")
endif()


# ice
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/ice")


# miniz
if(NOT TARGET miniz)
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/miniz")
endif()


# minizip-ng
if(NOT TARGET MINIZIP::minizip)
    set(MZ_COMPAT           OFF CACHE INTERNAL "")
    set(MZ_ZLIB             OFF CACHE INTERNAL "")
    set(MZ_BZIP2            OFF CACHE INTERNAL "")
    set(MZ_LZMA             OFF CACHE INTERNAL "")
    set(MZ_ZSTD             OFF CACHE INTERNAL "")
    set(MZ_LIBCOMP          OFF CACHE INTERNAL "")
    set(MZ_PKCRYPT          OFF CACHE INTERNAL "")
    set(MZ_WZAES            OFF CACHE INTERNAL "")
    set(MZ_OPENSSL          OFF CACHE INTERNAL "")
    set(MZ_FETCH_LIBS       ON  CACHE INTERNAL "")
    set(MZ_FORCE_FETCH_LIBS ON  CACHE INTERNAL "")
    set(SKIP_INSTALL_ALL    ON  CACHE INTERNAL "")
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/minizip-ng")
endif()


# stb
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/stb")
