cmake_minimum_required(VERSION 3.19)

add_library(persistance STATIC  src/persistance/data_storage_impl.cpp
                                src/persistance/create_component.cpp)
                    
target_include_directories(persistance
    PUBLIC 
        $<INSTALL_INTERFACE:inc>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    PRIVATE src
)

target_link_libraries(persistance config)