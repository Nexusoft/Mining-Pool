cmake_minimum_required(VERSION 3.19)

add_executable(nexus_http_interface_test nexus_http_interface_test.cpp)
target_link_libraries(
  nexus_http_interface_test
  gtest_main
  nexus_http_interface
  config
)

include(GoogleTest)
gtest_discover_tests(nexus_http_interface_test)