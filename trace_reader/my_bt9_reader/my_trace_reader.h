#ifndef TREAD_MY_2016_TRACE_READER
#define TREAD_MY_2016_TRACE_READER

#include <string>
#include <iostream>
#include <fstream>
#include "cbp_assert.h"
#include "tread.h"
#include <stdio.h>
#include <string.h>
#include <cstdio>
#include <map>
#include <vector>
#include <regex>
#include "pystring.h"

using namespace std;

typedef struct __attribute__((packed)) trace_meta_ {
unsigned long long svaddr;
unsigned long long tvaddr;
unsigned char packed_info;   
} stTraceMeta;

class branch_record_c1 : public branch_record_c
{
	public:
		bool taken;
	branch_record_c1() {
		memset((char *)this, 0, sizeof(branch_record_c1));
	}
};

class CMyGzTraceReader
{

private:
	ifstream *pFileIn;
	map<unsigned int, vector<long long>> mapNode;
	map<unsigned int, vector<long long>> mapEdge;
	map<string, int> mapSubType;
	map<string, int> mapDirct;
	map<string, int> mapCond;
public:	
	unsigned long long g_total_instr_count;

private:
	void parseNode( std::vector<std::string> & vec);
	void parseEdge( std::vector<std::string> & vec);

public:
	CMyGzTraceReader(char* fileName, char * bugz);
	~CMyGzTraceReader();
	bool preoperation();
	bool getLine(branch_record_c1 * br);
};


class CMyTraceReader
{

private:
	ifstream *pFileIn;

public:
	CMyTraceReader(char* fileName) {
		pFileIn = new ifstream(fileName, ios::in | ios::binary);
		CBP_ASSERT(pFileIn);
	};
	~CMyTraceReader() {
		if (pFileIn) {
			delete pFileIn;
		}
	};

	bool getLine(branch_record_c1 * br) {
		if (!pFileIn) {
			printf("trace file NOT opened !!");
		}
		if((pFileIn) && (!(pFileIn->eof()))) {
			stTraceMeta meta;
			pFileIn->read((char *) &meta, sizeof(stTraceMeta));
			br->instruction_addr = meta.svaddr;
			br->instruction_next_addr = meta.tvaddr;
			br->is_conditional = ((meta.packed_info>>1) & 0x3) == 0;
			br->is_indirect = ((meta.packed_info>>3) & 0x3) == 1;
			br->is_return = ((meta.packed_info >>5) & 0x3) == 0;
			br->is_call = ((meta.packed_info >>5) & 0x3) == 1;
			br->taken = meta.packed_info & 0x01;		
			return true;
		} else {
			return false;
		};
	};
};


#endif