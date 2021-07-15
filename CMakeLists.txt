cmake_minimum_required(VERSION 3.19)

project(NexusPool VERSION 1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

configure_file(src/version.h.in version.h)

include_directories(${CMAKE_SOURCE_DIR}/include/)
link_directories(${CMAKE_SOURCE_DIR}/libs)

option(WITH_TESTS "Build with unit tests" OFF)

find_package(Threads REQUIRED)

# OpenSSL
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

#ASIO
add_definitions(-DASIO_STANDALONE)
include_directories(${CMAKE_SOURCE_DIR}/include/asio-1.18.1/include)

if(UNIX)
    add_definitions(-DUNIX)
endif()

if(WIN32)
    add_definitions(-D_WIN32_WINT=0x0601 -DNOMINMAX -D_CRT_SECURE_NO_WARNINGS)
endif()


# add submodules
add_subdirectory(src/chrono)
add_subdirectory(src/network)
add_subdirectory(src/config)
add_subdirectory(src/api)
add_subdirectory(src/LLP)
add_subdirectory(src/LLC)
add_subdirectory(src/TAO)
add_subdirectory(src/persistance)

if(WITH_TESTS)
    add_subdirectory(tests)
endif()

add_executable(NexusPool 
                src/main.cpp 
                src/pool.cpp
                src/wallet_connection.cpp
                src/timer_manager.cpp 
                src/pool_manager.cpp
                src/miner_connection.cpp
                src/session.cpp)

target_include_directories(NexusPool PUBLIC "${PROJECT_BINARY_DIR}")

target_link_libraries(NexusPool chrono network config LLP TAO api persistance)

target_link_libraries(NexusPool Threads::Threads)
