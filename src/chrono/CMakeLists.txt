cmake_minimum_required(VERSION 3.19)

add_library(chrono STATIC "src/chrono/create_component.cpp")

target_include_directories(chrono
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
        $<INSTALL_INTERFACE:inc>
    PRIVATE src
)