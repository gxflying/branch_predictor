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

LOOP_TAIL_REPAIR ?= 0
PRED ?= 8k
PREDICTOR_PATH = ./AndreSeznecLimited/cbp8KB
ifeq ($(PRED),64k)
PREDICTOR_PATH = ./AndreSeznecLimited/cbp64KB
endif
ifeq ($(PRED),boom)
PREDICTOR_PATH = ./AndreSeznecLimited/sonicBoom
endif
ifeq ($(PRED),btfn)
PREDICTOR_PATH = ./AndreSeznecLimited/sonicBoom_btfn
endif

#LDLIBS   += -lboost_iostreams
LDLIBS   += -lboost_iostreams-mgw8-mt-d-x64-1_75

LDFLAGS += -L$(BOOST)/stage/lib -Wl,-rpath $(BOOST)/stage/lib

CPPFLAGS := -O3 -Wall -std=c++11 -Wextra -Winline -Winit-self -Wno-sequence-point\
           -Wno-unused-function -Wno-inline -fPIC -W -Wcast-qual -Wpointer-arith -Woverloaded-virtual\
           -I$(CBP_BASE) -I$(BOOST) -I$(BOOST)/iostreams -I$(BOOST)/iostreams/device -I.\
		   -DLOOP_TAIL_REPAIR=$(LOOP_TAIL_REPAIR) \
		   -I$(PREDICTOR_PATH)

PROGRAMS := predictor

objects = predictor.o main.o 

all: $(PROGRAMS)

SRC = main.cc $(PREDICTOR_PATH)/predictor.cc

#predictor : $(objects)
#	$(CXX) $(CPPFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

predictor : $(SRC)
	$(CXX) $(CPPFLAGS) $(LDFLAGS) $(SRC) -o predictor $(LDLIBS)

dbg: clean
	$(MAKE) DBG_BUILD=1 all

clean:
	rm -f $(PROGRAMS) $(objects)
run:
	reset; ./predictor ../../log/SHORT_SERVER-27.bt9.trace

# make clean; make PRED=boom LOOP_TAIL_REPAIR=0 -j 32
#
#
#
#