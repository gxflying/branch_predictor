cmake_minimum_required(VERSION 3.1)
set (CMAKE_C_COMPILER "gcc")
set (CMAKE_CXX_COMPILER "g++")

#add_subdirectory(math)

project(CPredictor)

#LINK_DIRECTORIES("D:/OSGEARTH/lib")
#LINK_LIBRARIES(osg osgDB osgViewer)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -g -O0" )
#add_compile_options(-std=c++11 -Wall -g -O0)

# Define executable target
include_directories("./3rd_part/pystring" ".")

aux_source_directory(. DIR_SRCS)
#set(SRCS cbp_inst.cc main.cc op_state.cc predictor.cc tread.cc ./3rd_part/pystring/pystring.cpp)

set(SRCS ${DIR_SRCS} ./3rd_part/pystring/pystring.cpp)
add_executable(CPredictor ${SRCS})

# target_link_libraries(CPredictor ${SDL2_LIBS} ${OPENGL_LIBRARIES})