cmake_minimum_required(VERSION 3.19)

add_library(TAO STATIC  src/Util/encoding.cpp
                        src/Util/memory.cpp 
                        src/Register/types/address.cpp)
                    
target_include_directories(TAO
    PUBLIC 
        $<INSTALL_INTERFACE:inc>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    PRIVATE src
)

target_link_libraries(TAO LLC OpenSSL::Crypto)