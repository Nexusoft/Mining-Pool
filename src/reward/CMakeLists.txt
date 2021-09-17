cmake_minimum_required(VERSION 3.19)

add_library(reward STATIC 
                          src/reward/create_component.cpp 
                          src/reward/component_impl.cpp 
                          src/reward/payout_manager.cpp)
                    
target_include_directories(reward
    PUBLIC 
        $<INSTALL_INTERFACE:inc>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    PRIVATE src
)

target_link_libraries(reward LLC LLP persistance nexus_http_interface)