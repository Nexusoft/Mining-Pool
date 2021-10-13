cmake_minimum_required(VERSION 3.19)

add_library(pool STATIC   src/pool/timer_manager.cpp 
                          src/pool/miner_connection.cpp 
                          src/pool/wallet_connection.cpp 
                          src/pool/pool_manager.cpp 
                          src/pool/session.cpp)
                    
target_include_directories(pool
    PUBLIC 
        $<INSTALL_INTERFACE:inc>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    PRIVATE src
)

target_link_libraries(pool chrono network config LLP TAO api persistance reward)