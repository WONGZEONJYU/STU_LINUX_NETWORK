cmake_minimum_required(VERSION 3.10)

project(file_server)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE "debug")

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)

message("Build directory: ${CMAKE_BINARY_DIR}")

if(NOT EXISTS ${CMAKE_BINARY_DIR})
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

include_directories(../11.msg_parser/)
include_directories(../12.tcp_client_frame/)
include_directories(../13.tcp_server_frame/)
include_directories(../18.udp_frame/)
include_directories(../19.client_base/)

add_executable(file_server 
            main.cpp
            page.cpp
            page.h
            response.cpp
            response.h
            ../11.msg_parser/message.cpp 
            ../11.msg_parser/msg_parser.cpp
            ../12.tcp_client_frame/tcp_client.cpp
            ../13.tcp_server_frame/tcp_server.cpp
            ../18.udp_frame/udp_point.cpp
            ../19.client_base/utility.cpp
            )

