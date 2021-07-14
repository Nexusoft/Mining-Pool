cmake_minimum_required(VERSION 3.19)

enable_testing()

add_executable(api_test "api_test.cpp" "api_fixture.hpp")
target_link_libraries(
  api_test
  gtest_main
  network
  api
  persistance
  config
)

include(GoogleTest)
gtest_discover_tests(api_test)