include_directories (${TEST_SOURCE_DIR}/include)
add_executable(main main.cpp)
target_link_libraries(main logger)
