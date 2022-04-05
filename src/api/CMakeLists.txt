cmake_minimum_required(VERSION 3.19)

add_library(api STATIC  src/api/component_impl.cpp 
                        src/api/create_component.cpp)
                    
target_include_directories(api
    PUBLIC 
        $<INSTALL_INTERFACE:inc>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    PRIVATE src
)

target_link_libraries(api spdlog config persistance TAO common oatpp chrono nlohmann_json::nlohmann_json asio)

if(WIN32)
target_link_libraries(api Ws2_32.lib)
endif()