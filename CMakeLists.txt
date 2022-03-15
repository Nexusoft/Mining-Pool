cmake_minimum_required(VERSION 3.19)

project(NexusPool VERSION 0.9 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

configure_file(src/version.h.in version.h)

include_directories(${CMAKE_SOURCE_DIR}/include/)
link_directories(${CMAKE_SOURCE_DIR}/libs)




option(WITH_TESTS "Build with unit tests" OFF)

find_package(Threads REQUIRED)

# OpenSSL
set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

#ASIO
add_definitions(-DASIO_STANDALONE)
include_directories(${CMAKE_SOURCE_DIR}/include/asio-1.18.1/include)

if(UNIX)
    add_definitions(-DUNIX)
    find_package(SQLite3 REQUIRED)
    if (SQLITE3_FOUND)
        include_directories(${SQLITE3_INCLUDE_DIRS})
    endif (SQLITE3_FOUND)
endif()

if(WIN32)
    add_definitions(-D_WIN32_WINT=0x0601 -DNOMINMAX -D_CRT_SECURE_NO_WARNINGS)
endif()

find_package(oatpp 1.3.0 REQUIRED)
find_package(oatpp-openssl 1.3.0 REQUIRED)

# add submodules
add_subdirectory(src/chrono)
add_subdirectory(src/common)
add_subdirectory(src/network)
add_subdirectory(src/config)
add_subdirectory(src/api)
add_subdirectory(src/LLP)
add_subdirectory(src/LLC)
add_subdirectory(src/TAO)
add_subdirectory(src/persistance)
add_subdirectory(src/reward)
add_subdirectory(src/nexus_http_interface)
add_subdirectory(src/pool)

if(WITH_TESTS)
    add_subdirectory(tests)
endif()

add_executable(NexusPool 
                src/main.cpp 
                src/pool.cpp)

target_include_directories(NexusPool PUBLIC "${PROJECT_BINARY_DIR}")

target_link_libraries(NexusPool pool)

target_link_libraries(NexusPool Threads::Threads)

set_target_properties(NexusPool PROPERTIES VS_DEBUGGER_COMMAND_ARGUMENTS "-a F:/Work/Nexus Project/LLL-HPP/out/build/x64-Debug/api.conf -p pool.conf")