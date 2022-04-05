cmake_minimum_required(VERSION 3.19)

add_library(common STATIC src/common/utils.cpp
                          src/common/pool_api_data_exchange_impl.cpp)
                    
target_include_directories(common
    PUBLIC 
        $<INSTALL_INTERFACE:inc>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    PRIVATE src
)

target_link_libraries(common date::date)
