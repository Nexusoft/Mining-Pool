cmake_minimum_required(VERSION 3.19)

add_library(network_mock INTERFACE)

target_include_directories(network_mock
    INTERFACE 
        $<INSTALL_INTERFACE:inc_mock>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc_mock>
)
target_link_libraries(config_mock INTERFACE
  network
  gmock
)
