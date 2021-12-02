cmake_minimum_required(VERSION 3.19)

add_library(pool_mock INTERFACE)

target_include_directories(pool_mock
    INTERFACE 
        $<INSTALL_INTERFACE:inc_mock>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc_mock>
)
target_link_libraries(pool_mock INTERFACE
  pool
  gmock
)
