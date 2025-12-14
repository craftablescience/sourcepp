macro(sourcepp_test_setup_pre)
    if(SOURCEPP_BUILD_TESTS)
        set(SOURCEPP_TEST_NAME "${PROJECT_NAME}_test")
        add_sourcepp_remote_library(googletest https://github.com/google/googletest main ALLOW_NOT_PINNED)
        enable_testing()
        set(${SOURCEPP_TEST_NAME}_SOURCES "")
        list(APPEND ${SOURCEPP_TEST_NAME}_DEPS gtest_main)

        # Download assets
        FetchContent_Declare(
                ${PROJECT_NAME}_assets
                GIT_REPOSITORY "https://github.com/craftablescience/sourcepp_assets.git"
                GIT_TAG "origin/main"
                GIT_SHALLOW ON
                SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/test/res")
        FetchContent_MakeAvailable(${PROJECT_NAME}_assets)
    endif()
endmacro()


macro(sourcepp_test_setup_post)
    if(SOURCEPP_BUILD_TESTS)
        add_executable(${SOURCEPP_TEST_NAME} ${${SOURCEPP_TEST_NAME}_SOURCES})
        target_link_libraries(${SOURCEPP_TEST_NAME} PUBLIC ${${SOURCEPP_TEST_NAME}_DEPS})
        target_compile_definitions(${SOURCEPP_TEST_NAME} PUBLIC ASSET_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/test/res/")
        if(SOURCEPP_BUILD_TESTS_EXTRA)
            target_compile_definitions(${SOURCEPP_TEST_NAME} PUBLIC SOURCEPP_BUILD_TESTS_EXTRA)
        endif()
        include(GoogleTest)
        gtest_discover_tests(${SOURCEPP_TEST_NAME})
    endif()
endmacro()
