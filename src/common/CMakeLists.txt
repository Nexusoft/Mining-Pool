cmake_minimum_required(VERSION 3.19)

add_library(common STATIC src/common/utils.cpp "inc/common/types.hpp")
                    
target_include_directories(common
    PUBLIC 
        $<INSTALL_INTERFACE:inc>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
)