cmake_minimum_required(VERSION 3.19)

add_library(common_mock INTERFACE)

target_include_directories(common_mock
    INTERFACE 
        $<INSTALL_INTERFACE:inc_mock>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc_mock>
)
target_link_libraries(common_mock INTERFACE
  common
  gmock
)
