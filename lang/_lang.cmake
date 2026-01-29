macro(sourcepp_lang_setup_pre)
    # C bindings
    if(SOURCEPP_BUILD_C_WRAPPERS)
        include_subdirectory(lang/c/src/sourceppc PROPAGATE ${PROJECT_NAME}c_SOURCES)
    endif()


    # Python bindings (pre)
    if(SOURCEPP_BUILD_PYTHON_WRAPPERS)
        set(${PROJECT_NAME}_PYTHON "${PROJECT_NAME}_python")
        find_package(Python REQUIRED
                COMPONENTS Interpreter Development.Module
                OPTIONAL_COMPONENTS Development.SABIModule)
        add_sourcepp_remote_library(nanobind https://github.com/wjakob/nanobind master ALLOW_NOT_PINNED)
        set(${${PROJECT_NAME}_PYTHON}_SOURCES "")
        set(${${PROJECT_NAME}_PYTHON}_DEFINES "")

        # Set the version and git commit hash here
        find_package(Git REQUIRED)
        execute_process(COMMAND ${GIT_EXECUTABLE} log -1 --format=%H
                WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                OUTPUT_VARIABLE SOURCEPP_GIT_TAG
                RESULT_VARIABLE SOURCEPP_GIT_TAG_ERROR
                OUTPUT_STRIP_TRAILING_WHITESPACE)
        if(NOT SOURCEPP_GIT_TAG)
            message(FATAL_ERROR "Failed to retrieve git commit SHA: ${SOURCEPP_GIT_TAG_ERROR}")
        endif()
        configure_file("${CMAKE_CURRENT_SOURCE_DIR}/lang/python/cfg/CMakeLists.txt" "${CMAKE_CURRENT_SOURCE_DIR}/lang/python/CMakeLists.txt" @ONLY)
        configure_file("${CMAKE_CURRENT_SOURCE_DIR}/lang/python/cfg/pyproject.toml" "${CMAKE_CURRENT_SOURCE_DIR}/lang/python/pyproject.toml")
        configure_file("${CMAKE_CURRENT_SOURCE_DIR}/lang/python/cfg/__init__.py"    "${CMAKE_CURRENT_SOURCE_DIR}/lang/python/src/${PROJECT_NAME}/__init__.py")
        configure_file("${CMAKE_CURRENT_SOURCE_DIR}/lang/python/cfg/__init__.py"    "${CMAKE_CURRENT_SOURCE_DIR}/lang/python/src/${PROJECT_NAME}/__init__.pyi")

        # These need to be inside the python directory, let's duplicate them!
        configure_file("${CMAKE_CURRENT_SOURCE_DIR}/LICENSE"                      "${CMAKE_CURRENT_SOURCE_DIR}/lang/python/LICENSE"                      COPYONLY)
        configure_file("${CMAKE_CURRENT_SOURCE_DIR}/README.md"                    "${CMAKE_CURRENT_SOURCE_DIR}/lang/python/README.md"                    COPYONLY)
        configure_file("${CMAKE_CURRENT_SOURCE_DIR}/THIRDPARTY_LEGAL_NOTICES.txt" "${CMAKE_CURRENT_SOURCE_DIR}/lang/python/THIRDPARTY_LEGAL_NOTICES.txt" COPYONLY)
    endif()


    # WASM bindings (pre)
    if(SOURCEPP_BUILD_WASM_WRAPPERS)
        set(${PROJECT_NAME}_WASM "${PROJECT_NAME}_wasm")
        set(${${PROJECT_NAME}_WASM}_DEPS    "")
        set(${${PROJECT_NAME}_WASM}_SOURCES "")
        set(${${PROJECT_NAME}_WASM}_DEFINES "")
    endif()
endmacro()


macro(sourcepp_lang_setup_post)
    # Python bindings (post)
    if(SOURCEPP_BUILD_PYTHON_WRAPPERS)
        nanobind_add_module(${${PROJECT_NAME}_PYTHON} NB_STATIC STABLE_ABI LTO
                "${CMAKE_CURRENT_SOURCE_DIR}/lang/python/src/${PROJECT_NAME}.cpp"
                ${${${PROJECT_NAME}_PYTHON}_SOURCES})
        set_target_properties(${${PROJECT_NAME}_PYTHON} PROPERTIES
                OUTPUT_NAME "_${PROJECT_NAME}_impl"
                LIBRARY_OUTPUT_DIRECTORY         "${CMAKE_CURRENT_SOURCE_DIR}/lang/python/src/${PROJECT_NAME}"
                LIBRARY_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_CURRENT_SOURCE_DIR}/lang/python/src/${PROJECT_NAME}"
                LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/lang/python/src/${PROJECT_NAME}")
        target_compile_definitions(${${PROJECT_NAME}_PYTHON} PRIVATE ${${${PROJECT_NAME}_PYTHON}_DEFINES})
        target_link_libraries(${${PROJECT_NAME}_PYTHON} PRIVATE ${${${PROJECT_NAME}_PYTHON}_DEPS})
        install(TARGETS ${${PROJECT_NAME}_PYTHON} LIBRARY DESTINATION "./${PROJECT_NAME}")

        add_custom_target(${${PROJECT_NAME}_PYTHON}_all)
        add_dependencies(${${PROJECT_NAME}_PYTHON}_all ${${PROJECT_NAME}_PYTHON})

        nanobind_add_stub(${${PROJECT_NAME}_PYTHON}_stubgen
                DEPENDS ${${PROJECT_NAME}_PYTHON}
                MODULE "${PROJECT_NAME}._${PROJECT_NAME}_impl"
                MARKER_FILE "${CMAKE_CURRENT_SOURCE_DIR}/lang/python/src/${PROJECT_NAME}/py.typed"
                RECURSIVE
                OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lang/python/src/${PROJECT_NAME}"
                PYTHON_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lang/python/src")
        add_dependencies(${${PROJECT_NAME}_PYTHON}_all ${${PROJECT_NAME}_PYTHON}_stubgen)
        install(CODE "
            file(GLOB_RECURSE ${${PROJECT_NAME}_PYTHON}_STUB_FILES \"${CMAKE_CURRENT_SOURCE_DIR}/lang/python/src/${PROJECT_NAME}/*.pyi\")
            list(APPEND ${${PROJECT_NAME}_PYTHON}_STUB_FILES \"${CMAKE_CURRENT_SOURCE_DIR}/lang/python/src/${PROJECT_NAME}/py.typed\")
            foreach(STUB_FILE \${${${PROJECT_NAME}_PYTHON}_STUB_FILES})
                file(RELATIVE_PATH REL_PATH \"${CMAKE_CURRENT_SOURCE_DIR}/lang/python/src/${PROJECT_NAME}\" \${STUB_FILE})
                get_filename_component(STUB_DIR \"\${REL_PATH}\" DIRECTORY)
                file(INSTALL \${STUB_FILE} DESTINATION \"\${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}/\${STUB_DIR}\")
            endforeach()
        ")
    endif()


    # WASM bindings (post)
    if(SOURCEPP_BUILD_WASM_WRAPPERS)
        add_executable(${${PROJECT_NAME}_WASM} "${CMAKE_CURRENT_SOURCE_DIR}/lang/wasm/src/${PROJECT_NAME}.cpp" ${${${PROJECT_NAME}_WASM}_SOURCES})
        target_compile_definitions(${${PROJECT_NAME}_WASM} PRIVATE ${${${PROJECT_NAME}_WASM}_DEFINES})
        target_link_libraries(${${PROJECT_NAME}_WASM} PRIVATE embind ${${${PROJECT_NAME}_WASM}_DEPS})
    endif()
endmacro()
