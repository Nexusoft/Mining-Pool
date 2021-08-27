cmake_minimum_required(VERSION 3.19)

link_directories(${CMAKE_SOURCE_DIR}/libs)

enable_testing()

add_executable(api_test "api_test.cpp")
target_link_libraries(
  api_test
  gtest_main
  api
)

include(GoogleTest)
gtest_discover_tests(api_test)