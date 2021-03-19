# Copyright 2015 Samsung Austin Semiconductor, LLC.

# Description: Makefile for building a cbp 2016 submission.
#
# The following commands and Makefile worked for CENTOS v7:
# =========================================================
# yum install gcc-c++
# yum install boost
# yum install boost-devel.x86_64
#
# The following commands worked for Ubuntu:
#==========================================
#  sudo apt-get update
#  sudo apt-get install libboost-all-dev
#
#  installed it in /usr/include/boost
#  lib at /usr/lib/x86_64-linux-gnu/
#
#  locate boost | grep lib
#
#  sudo apt-get install g++
#    Installed 4.8


# Include path to Boost area on local machine
#
#worked with both CENTOS and Ubuntu: export BOOST    := /usr/
export BOOST    := ../../../../000_3rdpart_opensource/boost_1_75_0
#export BOOST    := /sarc/spa/tools/conda/envs/spa-1




ifndef BOOST
$(error "You must define BOOST")
endif


export CBP_BASE := $(CURDIR)/..
CXX := g++

OBJDIR := $(CBP_BASE)/bin
SRC    := $(CBP_BASE)/sim
VPATH  := $(SRC)

#LDLIBS   += -lboost_iostreams
LDLIBS   += -lboost_iostreams-mgw8-mt-d-x64-1_75

LDFLAGS += -L$(BOOST)/stage/lib -Wl,-rpath $(BOOST)/stage/lib

CPPFLAGS := -O3 -Wall -std=c++11 -Wextra -Winline -Winit-self -Wno-sequence-point\
           -Wno-unused-function -Wno-inline -fPIC -W -Wcast-qual -Wpointer-arith -Woverloaded-virtual\
           -I$(CBP_BASE) -I$(BOOST) -I$(BOOST)/iostreams -I$(BOOST)/iostreams/device

PROGRAMS := predictor

objects = predictor.o main.o 

all: $(PROGRAMS)

predictor : $(objects)
	$(CXX) $(CPPFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

dbg: clean
	$(MAKE) DBG_BUILD=1 all

clean:
	rm -f $(PROGRAMS) $(objects)
run:
	reset; ./predictor.exe ../../../trace_log_from_BPC/evaluationTraces.Final-2016/evaluationTraces/SHORT_SERVER-21.bt9.trace