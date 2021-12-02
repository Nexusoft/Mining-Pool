cmake_minimum_required(VERSION 3.19)

add_library(persistance_mock INTERFACE)

target_include_directories(persistance_mock
    INTERFACE 
        $<INSTALL_INTERFACE:inc_mock>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc_mock>
)
target_link_libraries(persistance_mock INTERFACE
  persistance
  gmock
)
