cmake_minimum_required(VERSION 3.19)

link_directories(${CMAKE_SOURCE_DIR}/libs)

enable_testing()

add_executable(api_test "api_test.cpp" "api_fixture.hpp")
target_link_libraries(
  api_test
  gtest_main
  network
  api
  persistance
  config
  sqlite3
)

include(GoogleTest)
gtest_discover_tests(api_test)