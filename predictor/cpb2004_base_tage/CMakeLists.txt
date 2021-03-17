cmake_minimum_required(VERSION 3.1)
set (CMAKE_C_COMPILER "gcc")
set (CMAKE_CXX_COMPILER "g++")


#SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin)       #设置可执行文件的输出目录
#SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../lib)           #设置库文件的输出目录

project(CPredictor)


add_subdirectory("../../3rd_part/pystring" binary_dir)

########## build time lib setting
#LINK_DIRECTORIES("/ddd/sss")           # build time lib check path
#LINK_LIBRARIES(osg osgDB osgViewer)    #for already exsist lib
########## run time lib setting
# TO DO  HOW ??


set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -g -O0 ${CMAKE_CXX_FLAGS}" )
message (STATUS "------------------------- ${CMAKE_CXX_FLAGS}")
#add_compile_options(-std=c++11 -Wall -g -O0)

# add header file path
include_directories("../../3rd_part/pystring"  "../../trace_reader" "../../framework" ".")


# get source files in current dir
aux_source_directory(. PREDICTOR_SRCS)
aux_source_directory(../../framework FRAMEWORK_SRCS)
aux_source_directory(../../trace_reader TRACE_READER_SRCS)

# Define executable target
#set(SRCS cbp_inst.cc main.cc op_state.cc predictor.cc tread.cc ./3rd_part/pystring/pystring.cpp)
message (STATUS "xxxx ${DIR_SRCS}")
add_executable(CPredictor ${PREDICTOR_SRCS} ${FRAMEWORK_SRCS} ${TRACE_READER_SRCS})

#set(SRCS ${DIR_SRCS} ./3rd_part/pystring/pystring.cpp)
#add_executable(CPredictor ${SRCS})

target_link_libraries(CPredictor pystring)  # for lib from source code


