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


# hat-trie
if(NOT TARGET tsl::hat_trie)
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/hat-trie")
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
    set(MZ_FETCH_LIBS       ON  CACHE INTERNAL "")
    set(MZ_FORCE_FETCH_LIBS ON  CACHE INTERNAL "")
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


# OpenCL
if(SOURCEPP_BUILD_WITH_OPENCL AND NOT TARGET OpenCL::OpenCL)
    find_package(OpenCL)
    if(NOT OpenCL_FOUND)
        set(SOURCEPP_BUILD_WITH_OPENCL OFF CACHE INTERNAL "" FORCE)
    endif()
endif()

function(sourcepp_add_opencl TARGET)
    if(SOURCEPP_BUILD_WITH_OPENCL)
        target_compile_definitions(${TARGET} PRIVATE SOURCEPP_BUILD_WITH_OPENCL)
        target_link_libraries(${TARGET} PRIVATE OpenCL::OpenCL)
    endif()
endfunction()


# stb
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/stb")


# TBB
function(sourcepp_add_tbb TARGET)
    if(SOURCEPP_BUILD_WITH_TBB)
        target_compile_definitions(${TARGET} PRIVATE SOURCEPP_BUILD_WITH_TBB)
        if(NOT MSVC)
            target_link_libraries(${TARGET} PRIVATE tbb)
        endif()
    endif()
endfunction()
