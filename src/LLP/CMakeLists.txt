cmake_minimum_required(VERSION 3.19)

add_library(LLP STATIC src/LLP/block.cpp)
target_include_directories(LLP
    PUBLIC 
        $<INSTALL_INTERFACE:inc>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>)

target_link_libraries(LLP network LLC)