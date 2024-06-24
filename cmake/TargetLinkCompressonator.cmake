function(target_link_compressonator TARGET)
    if(MSVC)
        if(SOURCEPP_USE_STATIC_MSVC_RUNTIME)
            target_link_libraries(${TARGET} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/thirdparty/compressonator/lib/win64/Compressonator_MT$<$<CONFIG:Debug>:d>.lib")
        else()
            target_link_libraries(${TARGET} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/thirdparty/compressonator/lib/win64/Compressonator_MD$<$<CONFIG:Debug>:d>.lib")
        endif()
    elseif(UNIX)
        target_link_libraries(${TARGET} PRIVATE
                "${CMAKE_CURRENT_SOURCE_DIR}/src/thirdparty/compressonator/lib/linux64/libCMP_Compressonator.a"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/thirdparty/compressonator/lib/linux64/libCMP_Core.a"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/thirdparty/compressonator/lib/linux64/libCMP_Core_AVX.a"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/thirdparty/compressonator/lib/linux64/libCMP_Core_AVX512.a"
                "${CMAKE_CURRENT_SOURCE_DIR}/src/thirdparty/compressonator/lib/linux64/libCMP_Core_SSE.a")
    else()
        message(FATAL_ERROR "Unable to link to Compressonator library!")
    endif()
    target_include_directories(${TARGET} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/thirdparty/compressonator/include")
endfunction()
