cmake_minimum_required(VERSION 3.19)

add_library(api STATIC  src/api/server.cpp)
                    
target_include_directories(api
    PUBLIC 
        $<INSTALL_INTERFACE:inc>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    PRIVATE src
)

target_link_libraries(api config persistance TAO common oatpp)