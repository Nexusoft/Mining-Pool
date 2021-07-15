cmake_minimum_required(VERSION 3.19)

enable_testing()

add_executable(reward_test reward_test.cpp)
target_link_libraries(
  reward_test
  gtest_main
)

include(GoogleTest)
gtest_discover_tests(reward_test)