cmake_minimum_required(VERSION 3.19)

enable_testing()

add_executable(persistance_test persistance_test.cpp)
target_link_libraries(
  persistance_test
  gtest_main
  persistance
)

include(GoogleTest)
gtest_discover_tests(persistance_test)