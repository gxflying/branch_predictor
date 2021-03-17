# Author: Jared Stark;   Created: Mon Aug 16 11:28:20 PDT 2004
# Description: Makefile for building a cbp submission.



objects = cbp_inst.o main.o op_state.o predictor.o tread.o pystring.o

PYSTRING_INC = ./3rd_part/pystring
PYSTRING_HEADER = ./3rd_part/pystring/pystring.h
PYSTRING_SRC = ./3rd_part/pystring/pystring.cpp

CFLAGS = -g -O3 -Wall 
CXXFLAGS = -g -O0 -Wall  -I$(PYSTRING_INC)

predictor : $(objects)
	$(CXX) $(CXXFLAGS) -o $@ $(objects)

cbp_inst.o : cbp_inst.h cbp_assert.h cbp_fatal.h cond_pred.h finite_stack.h indirect_pred.h stride_pred.h value_cache.h
main.o : tread.h cbp_inst.h predictor.h op_state.h my_trace_reader.h $(PYSTRING_HEADER)
op_state.o : op_state.h
predictor.o : predictor.h op_state.h tread.h cbp_inst.h
tread.o : tread.h cbp_inst.h op_state.h
pystring.o : $(PYSTRING_HEADER)
	$(CXX) -c -I./3rd_part/pystring ./3rd_part/pystring/pystring.cpp 

unzip:
	"C:\\Program Files\\7-Zip\\7z.exe" e $(gz)

parser:
	python3 ../trace_log_from_BPC/evaluationTraces.Final/main.py $(log) $(out)

run:
	./predictor.exe ../trace_log_from_BPC/evaluationTraces.Final/0.txt.bin
	./predictor.exe  ../trace_log_from_BPC/evaluationTraces.Final/evaluationTraces/SHORT_SERVER-10.bt9.trace
	./predictor.exe  ../trace_log_from_BPC/evaluationTraces.Final/evaluationTraces/SHORT_MOBILE-101.bt9.trace

	make clean; make;  ./predictor.exe ../trace_log_from_BPC/evaluationTraces.Final/SHORT_MOBILE-101.bt9.trace 0
	cmake -G"Unix Makefiles" ../
	./CPredictor.exe ../../trace_log_from_BPC/evaluationTraces.Final/SHORT_MOBILE-101.bt9.trace 1

.PHONY : clean
clean :
	rm -f predictor $(objects)

