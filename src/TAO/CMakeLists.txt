cmake_minimum_required(VERSION 3.19)

add_library(TAO STATIC  src/Util/encoding.cpp
                        src/Util/memory.cpp "inc/Register/types/address.h" "src/Register/types/address.cpp")
                    
target_include_directories(TAO
    PUBLIC 
        $<INSTALL_INTERFACE:inc>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
)

target_link_libraries(TAO LLC OpenSSL::Crypto)