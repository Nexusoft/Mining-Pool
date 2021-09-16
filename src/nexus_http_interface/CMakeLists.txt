cmake_minimum_required(VERSION 3.19)

add_library(nexus_http_interface STATIC 
                          
                          src/nexus_http_interface/create_component.cpp
                          src/nexus_http_interface/component_impl.cpp)
                    
target_include_directories(nexus_http_interface
    PUBLIC 
        $<INSTALL_INTERFACE:inc>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    PRIVATE src
)

target_link_libraries(nexus_http_interface common oatpp)