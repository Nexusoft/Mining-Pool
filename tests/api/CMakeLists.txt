cmake_minimum_required(VERSION 3.19)

link_directories(${CMAKE_SOURCE_DIR}/libs)

enable_testing()

add_executable(api_test "api_test.cpp")
target_link_libraries(
  api_test
  gtest_main
  api
)

add_custom_command(
        TARGET api_test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/NexusWebUI/tools/create_test_sqlite.py
                ${CMAKE_CURRENT_BINARY_DIR}/create_test_sqlite.py)

add_custom_command(
        TARGET api_test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/NexusWebUI/tools/create_test_data.py
                ${CMAKE_CURRENT_BINARY_DIR}/create_test_data.py)

include(GoogleTest)
gtest_discover_tests(api_test)