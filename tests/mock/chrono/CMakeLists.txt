cmake_minimum_required(VERSION 3.19)

add_library(chrono_mock INTERFACE)

target_include_directories(chrono_mock
    INTERFACE 
        $<INSTALL_INTERFACE:inc_mock>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc_mock>
)
target_link_libraries(chrono_mock INTERFACE
  chrono
  gmock
)
