include_directories (${TEST_SOURCE_DIR}/include)
add_executable(benchmark benchmark.cpp)
target_link_libraries(benchmark logger)

add_executable(installTest installTest.cpp)
target_link_libraries(installTest /usr/local/lib/liblogger.a)
