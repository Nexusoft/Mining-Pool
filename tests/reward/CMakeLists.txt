cmake_minimum_required(VERSION 3.19)

add_executable(reward_test reward_test.cpp "test_data.hpp")
target_link_libraries(reward_test
  gtest_main
  reward
  LLP
  nexus_http_interface_mock
  persistance_mock
)

include(GoogleTest)
gtest_discover_tests(reward_test)