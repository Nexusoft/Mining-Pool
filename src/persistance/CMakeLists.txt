cmake_minimum_required(VERSION 3.19)

add_library(persistance STATIC  src/persistance/sqlite/data_storage_impl.cpp
                                src/persistance/create_component.cpp 
                                src/persistance/component_impl.cpp 
                                src/persistance/storage_manager_impl.cpp
                                src/persistance/data_reader_impl.cpp
                                src/persistance/command/command_impl.cpp)
                    
target_include_directories(persistance
    PUBLIC 
        $<INSTALL_INTERFACE:inc>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    PRIVATE src
)

target_link_libraries(persistance config sqlite3)