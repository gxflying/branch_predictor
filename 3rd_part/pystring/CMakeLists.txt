cmake_minimum_required(VERSION 3.1)
set (CMAKE_C_COMPILER "gcc")
set (CMAKE_CXX_COMPILER "g++")

#add_subdirectory(math)

#project(pystring)


set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -g -O0" )
add_compile_options(-std=c++11 -Wall -g -O0)

# Define executable target

#include_directories( "./inc")
aux_source_directory(. DIR_SRCS)
#set(SRCS cbp_inst.cc main.cc op_state.cc predictor.cc tread.cc ./3rd_part/pystring/pystring.cpp)

message (STATUS "xxxx ${DIR_SRCS}")
#dynamic lib
#SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})           #设置库文件的输出目录
#add_library(pystring SHARED ${DIR_SRCS})
#static lib
add_library(pystring ${DIR_SRCS})
