cmake_minimum_required(VERSION 3.19)

add_library(TAO STATIC  src/TAO/Util/encoding.cpp
                        src/TAO/Util/memory.cpp 
                        src/TAO/Register/types/address.cpp
                        src/TAO/Ledger/difficulty.cpp
                        src/TAO/Ledger/prime.cpp)

                    
target_include_directories(TAO
    PUBLIC 
        $<INSTALL_INTERFACE:inc>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    PRIVATE src
)

target_link_libraries(TAO LLC OpenSSL::Crypto)
if(WIN32)
    target_link_libraries(TAO crypt32.lib)
endif()