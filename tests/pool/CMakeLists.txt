cmake_minimum_required(VERSION 3.19)

add_executable(pool_test miner_connection_test.cpp 
						llp_test.cpp)

target_link_libraries(pool_test
  gtest_main
  pool
  LLP
  LLC
  pool_mock
  network_mock
)

include(GoogleTest)
gtest_discover_tests(pool_test)