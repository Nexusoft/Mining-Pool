cmake_minimum_required(VERSION 3.19)

add_library(pool STATIC   src/pool/timer_manager_wallet.cpp 
                          src/pool/miner_connection_impl.cpp 
                          src/pool/wallet_connection_impl.cpp 
                          src/pool/pool_manager_impl.cpp 
                          src/pool/session_impl.cpp
                          src/pool/miner_connection_legacy_impl.cpp)
                    
target_include_directories(pool
    PUBLIC 
        $<INSTALL_INTERFACE:inc>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    PRIVATE src
)

target_link_libraries(pool chrono network config LLP TAO api persistance reward common)