include_directories (${TEST_SOURCE_DIR}../include)
add_executable(benchmark benchmark.cc)
target_link_libraries(benchmark logger)

add_executable(unittest unittest.cc)
target_link_libraries(unittest logger)
