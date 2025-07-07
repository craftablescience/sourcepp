# Create executable
add_executable(${PROJECT_NAME}_cwrap
        "${CMAKE_CURRENT_LIST_DIR}/cwrap.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/cwrap_tokens.h")
target_link_libraries(${PROJECT_NAME}_cwrap PRIVATE ${PROJECT_NAME}::kvpp)
