cmake_minimum_required(VERSION 3.10)

project(tcp_server_frame)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE "debug")

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)

message("Build directory: ${CMAKE_BINARY_DIR}")

if(NOT EXISTS ${CMAKE_BINARY_DIR})
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

include_directories(./)

add_executable(tcp_server_frame 
            server.cpp
            tcp_server.cpp
            tcp_client.cpp
            message.cpp 
            msg_parser.cpp)
