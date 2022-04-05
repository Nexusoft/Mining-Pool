cmake_minimum_required(VERSION 3.19)

add_executable(persistance_test persistance_test.cpp)
target_link_libraries(
  persistance_test
  gtest_main
  persistance
  sqlite3
  asio
)

add_custom_command(
        TARGET persistance_test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/NexusWebUI/tools/create_test_sqlite.py
                ${CMAKE_CURRENT_BINARY_DIR}/create_test_sqlite.py)

add_custom_command(
        TARGET persistance_test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/NexusWebUI/tools/create_test_data.py
                ${CMAKE_CURRENT_BINARY_DIR}/create_test_data.py)

add_custom_command(
        TARGET persistance_test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/dbschema_sqlite.sql
                ${CMAKE_CURRENT_BINARY_DIR}/dbschema_sqlite.sql)

include(GoogleTest)
gtest_discover_tests(persistance_test)