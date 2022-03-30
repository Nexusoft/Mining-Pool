cmake_minimum_required(VERSION 3.19)

project(NexusPool VERSION 1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

configure_file(src/version.h.in version.h)

set(CPM_DOWNLOAD_VERSION 0.35.0) 
set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")

if(NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
    message(STATUS "Downloading CPM.cmake")
    file(DOWNLOAD https://github.com/TheLartians/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake ${CPM_DOWNLOAD_LOCATION})
endif()

include(${CPM_DOWNLOAD_LOCATION})

find_package(Threads REQUIRED)

#spdlog
CPMAddPackage(NAME spdlog GITHUB_REPOSITORY gabime/spdlog VERSION 1.9.2)

#nlohmann json
CPMAddPackage(NAME nlohmann_json GITHUB_REPOSITORY nlohmann/json  VERSION 3.10.5)

#ASIO
CPMAddPackage("gh:chriskohlhoff/asio#asio-1-18-1@1.18.1")

# ASIO doesn't use CMake, we have to configure it manually. Extra notes for using on Windows:
#
# 1) If _WIN32_WINNT is not set, ASIO assumes _WIN32_WINNT=0x0501, i.e. Windows XP target, which is
# definitely not the platform which most users target.
#
# 2) WIN32_LEAN_AND_MEAN is defined to make Winsock2 work.
if(asio_ADDED)
  add_library(asio INTERFACE)

  target_include_directories(asio SYSTEM INTERFACE ${asio_SOURCE_DIR}/asio/include)
  target_compile_definitions(asio INTERFACE ASIO_STANDALONE ASIO_NO_DEPRECATED)
  target_link_libraries(asio INTERFACE Threads::Threads)

  if(WIN32)
    # macro see @ https://stackoverflow.com/a/40217291/1746503
    macro(get_win32_winnt version)
      if(CMAKE_SYSTEM_VERSION)
        set(ver ${CMAKE_SYSTEM_VERSION})
        string(REGEX MATCH "^([0-9]+).([0-9])" ver ${ver})
        string(REGEX MATCH "^([0-9]+)" verMajor ${ver})
        # Check for Windows 10, b/c we'll need to convert to hex 'A'.
        if("${verMajor}" MATCHES "10")
          set(verMajor "A")
          string(REGEX REPLACE "^([0-9]+)" ${verMajor} ver ${ver})
        endif("${verMajor}" MATCHES "10")
        # Remove all remaining '.' characters.
        string(REPLACE "." "" ver ${ver})
        # Prepend each digit with a zero.
        string(REGEX REPLACE "([0-9A-Z])" "0\\1" ver ${ver})
        set(${version} "0x${ver}")
      endif()
    endmacro()

    if(NOT DEFINED _WIN32_WINNT)
      get_win32_winnt(ver)
      set(_WIN32_WINNT ${ver})
    endif()

    message(STATUS "Set _WIN32_WINNET=${_WIN32_WINNT}")

    target_compile_definitions(asio INTERFACE _WIN32_WINNT=${_WIN32_WINNT} WIN32_LEAN_AND_MEAN)
  endif()
endif()

# Howard Hinnant's date library
CPMAddPackage(NAME date GITHUB_REPOSITORY HowardHinnant/date VERSION 3.0.1)

include_directories(${CMAKE_SOURCE_DIR}/include/)
link_directories(${CMAKE_SOURCE_DIR}/libs)

option(WITH_TESTS "Build with unit tests" OFF)

# OpenSSL
set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

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

add_subdirectory(oatpp-1.3.0)

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

target_link_libraries(NexusPool pool asio)

target_link_libraries(NexusPool Threads::Threads)

set_target_properties(NexusPool PROPERTIES VS_DEBUGGER_COMMAND_ARGUMENTS "-a F:/Work/Nexus Project/LLL-HPP/out/build/x64-Debug/api.conf -p pool.conf")