include_directories (${TEST_SOURCE_DIR}../include)
add_executable(benchmark benchmark.cpp)
target_link_libraries(benchmark logger)

add_executable(unittest unittest.cpp)
target_link_libraries(unittest logger)
