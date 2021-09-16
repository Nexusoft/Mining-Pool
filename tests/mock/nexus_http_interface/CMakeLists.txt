cmake_minimum_required(VERSION 3.19)

add_library(nexus_http_interface_mock INTERFACE)

target_include_directories(nexus_http_interface_mock
    INTERFACE 
        $<INSTALL_INTERFACE:inc_mock>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc_mock>
)
target_link_libraries(nexus_http_interface_mock INTERFACE
  nexus_http_interface
  gmock
)
