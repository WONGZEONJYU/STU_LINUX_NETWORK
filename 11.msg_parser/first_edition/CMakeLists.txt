cmake_minimum_required(VERSION 3.10)

project(test)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE "debug")

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)

message("Build directory: ${CMAKE_BINARY_DIR}")

if(NOT EXISTS ${CMAKE_BINARY_DIR})
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

include_directories(..)

add_executable( test test.cpp msg_parser.cpp ../message.cpp )

