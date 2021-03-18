#include "my_trace_reader.h"	

    void CMyGzTraceReader::parseNode(std::vector<std::string> & vec) {
		//#NODE   id      virtual_address    physical_address          opcode  size 
		//NODE    0                    0                    -                0    0      
		//NODE    1         0x558eb0ecfc           0x60de1cfc       0x54fffd28    4    class:  JMP+DIR+CND		
		//0       1         2                      3                4             5    6        7
		//int stoi (const wstring& str, size_t* idx = 0, int base = 10);
//*
		//printf("%s %s %s %s %s \r\n", vec[0].c_str(), vec[1].c_str(), vec[2].c_str(), vec[3].c_str(), vec[4].c_str());
		string v1 = pystring::strip(vec[1]);
		string v2 = pystring::strip(vec[2]);
		string v3 = pystring::strip(vec[3]);

		unsigned int id_ = stoi(v1);
		std::vector<long long> itemV_ ;
		long long vsaddr = stoull(v2, NULL, 16);
		long long psaddr = v3.compare("-")==0  ? 0 : stoull(v3, NULL, 16);
		long long br_info = 0;
		if (vec.size() >= 8) {
			string v7 = pystring::strip(vec[7]);
			std::vector<std::string> vec_br_type;			
			pystring::split(v7, vec_br_type, "+");
			br_info = (mapSubType[vec_br_type[0]]<<4) + (mapDirct[vec_br_type[1]]<<2) + (mapCond[vec_br_type[2]]);
		}
		itemV_.insert(itemV_.end(), vsaddr);
		itemV_.insert(itemV_.end(), psaddr);
		itemV_.insert(itemV_.end(), br_info);

		mapNode.insert(std::make_pair(id_, itemV_));

		//*/
	};

	void CMyGzTraceReader::parseEdge(std::vector<std::string> & vec) {
		///*
		//#EDGE    id  src_id   dest_id  taken      br_virt_target       br_phy_target   inst_cnt 
		//EDGE      0      0      1        N                    0                    -       23 	traverse_cnt:        1  
		//0         1      2      3        4         5                   6               7          8                    9 
		//printf("%s %s %s %s %s \r\n", vec[0].c_str(), vec[1].c_str(), vec[2].c_str(), vec[3].c_str(), vec[4].c_str());
		string v1 = pystring::strip(vec[1]);
		string v2 = pystring::strip(vec[2]);
		string v3 = pystring::strip(vec[3]);
		string v4 = pystring::strip(vec[4]);
		string v5 = pystring::strip(vec[5]);
		string v6 = pystring::strip(vec[6]);
		unsigned int id_ = stoi(v1);
		std::vector<long long>  itemV_;
		long long vtaddr = stoull(v5, NULL, 16);
		long long ptaddr = v6.compare("-")==0  ? 0 : stoull(v6, NULL, 16);
		long long src_id = stoull(v2);
		long long dest_id = stoull(v3);
		long long taken = v4.compare("T")==0 ? 1 : 0;
		itemV_.insert(itemV_.end(), vtaddr);
		itemV_.insert(itemV_.end(), ptaddr);
		itemV_.insert(itemV_.end(), src_id);	
		itemV_.insert(itemV_.end(), dest_id);	
		itemV_.insert(itemV_.end(), taken);	
		mapEdge.insert(std::make_pair(id_, itemV_));

		//*/
	};

    	CMyGzTraceReader::CMyGzTraceReader(char* fileName, char * bugz) {
		char trace_name_copy[256];
		char trnm_cmdline[256];
		memset((char *) trace_name_copy,  0, 256);
		memset((char *) trnm_cmdline,  0, 256);
		// we need the name the name of the trace 
		//CBP_ASSERT(fileName);
		if(bugz[0] == '1') {
			strcpy(trace_name_copy, fileName);
			sprintf(trnm_cmdline, "\"C:\\Program Files\\7-Zip\\7z.exe\" e %s.gz ", trace_name_copy);
			system(trnm_cmdline);
		}
		pFileIn = new ifstream(pystring::os::path::basename(fileName).c_str(), ios::in );
		printf("filename : %s  \r\n", pystring::os::path::basename(fileName).c_str());
		if(!pFileIn) {
			printf("open file error\n");
		}
		//SUBTYPE_MAP = {"RET":0, "CALL":1, "JMP":2, "NONE":3}
		//DIRECT_MAP  = {"DIR":0, "IND":1, "NONE":3}
		//COND_MAP    = {"CND":0, "UCD":1, "NONE":3}
		mapSubType.insert(std::make_pair("RET", 0));
		mapSubType.insert(std::make_pair("CALL", 1));
		mapSubType.insert(std::make_pair("JMP", 2));
		mapSubType.insert(std::make_pair("NONE", 3));
		mapDirct.insert(std::make_pair("DIR", 0));
		mapDirct.insert(std::make_pair("IND", 1));
		mapDirct.insert(std::make_pair("NONE", 2));
		mapCond.insert(std::make_pair("CND", 0));
		mapCond.insert(std::make_pair("UCD", 1));
		mapCond.insert(std::make_pair("NONE", 2));

	};
	CMyGzTraceReader::~CMyGzTraceReader() {
		if (pFileIn) {
			delete pFileIn;
		}

	};
	bool CMyGzTraceReader::preoperation()
	{
		volatile bool bNodeStart = false;
		volatile bool bEdgeStart = false;
		volatile bool bInstrSteamStart = false;
		volatile int loop = 0;
		//char line_t[1024];
		//while (pFileIn->getline (line_t, 1024))
		string line;
        while (getline (*pFileIn, line))
		{
			loop++;
			std::vector<std::string> vec;			
			pystring::split(line, vec, "");

			string token = "total_instruction_count:";
            auto gzip_suffix_pos = line.find(token);
            if (gzip_suffix_pos != std::string::npos) {
                auto pos = line.find_first_of('#');
                if (pos != std::string::npos) {
                    line.erase(pos, std::numeric_limits<std::string::size_type>::max());
					printf("line : %s\n", line.c_str());
                }
				line.erase(0, token.size());				
				printf("line : %s\n", line.c_str());

				g_total_instr_count = strtoull(line.c_str() , NULL, 0);
			}

			const char * lineBuf = line.c_str();
			volatile bool bNodeStr = lineBuf[0]=='N' && lineBuf[1]=='O' && lineBuf[2]=='D' && lineBuf[3]=='E';
			volatile bool bEdgeStr = lineBuf[0]=='E' && lineBuf[1]=='D' && lineBuf[2]=='G' && lineBuf[3]=='E';


			//printf("a -> %d %d %s\r\n", bNodeStr, bEdgeStr, lineBuf);
			if(!bNodeStart && bNodeStr) {   		// FOR MODE lines
				bNodeStart = true;
			} 
			if (!bEdgeStart && bEdgeStr) {   // FOR EDGE lines
				bEdgeStart = true;
			} 
			if(bNodeStart && !bNodeStr) {     
				bNodeStart = false;
			} 
			if(bEdgeStart && !bEdgeStr) {
				bEdgeStart = false;
				bInstrSteamStart = true;
				break;
			}
		
			if(bNodeStart) {
				parseNode(vec);
			} 
			if(bEdgeStart) {
				parseEdge(vec);
			}

		}
		//printf("b3 -> %d %d %d  \r\n", loop, bNodeStart, bEdgeStart);

	};

	bool CMyGzTraceReader::getLine(branch_record_c1 * br) {
		std::string line = "";
		//*
		static int loop;
		//while (getline (*pFileIn, line))
		if (getline (*pFileIn, line))
		{
			//printf("-- %s \n", line.c_str());
			//if(loop++ > 600) break;
			if(!pystring::isdigit(line)) {
				return 0;
			}


			// mapEdge ->  vtaddr ptaddr src_id dest_id taken
			unsigned int stream_id = stoull(pystring::strip(line));
			//printf("%s : ", line.c_str() );
			unsigned long long vtaddr = mapEdge[stream_id][0];
			unsigned long long ptaddr = mapEdge[stream_id][1];
			unsigned long long src_id = mapEdge[stream_id][2];
			unsigned long long dest_id = mapEdge[stream_id][3];
			bool actrual_taken = mapEdge[stream_id][4];

			//(mapSubType[vec_br_type[0]]<<4) + (mapDirct[vec_br_type[1]]<<2) + (mapCond[vec_br_type[2]]);
			//mapNode -> vsaddr psaddr br_info [mapSubType, mapDirct, mapCond]
			unsigned long long vsaddr = mapNode[src_id][0];
			unsigned long long psaddr = mapNode[src_id][1];
			unsigned long long br_info = mapNode[src_id][2];
			unsigned char br_subType = (br_info >> 4) & 0x3;
			unsigned char br_direct = (br_info >> 2) & 0x3;
			unsigned char br_cond = (br_info) & 0x3;

			//printf("%s ->  %lld %lld %llx %llx %lld \r\n", line.c_str(), src_id, dest_id, vtaddr, ptaddr, actrual_taken);
			//printf("%d ->  %lld %lld %lld \r\n", loop++, src_id, dest_id, actrual_taken);

			//SUBTYPE_MAP = {"RET":0, "CALL":1, "JMP":2, "NONE":3}
			//DIRECT_MAP  = {"DIR":0, "IND":1, "NONE":3}
			//COND_MAP    = {"CND":0, "UCD":1, "NONE":3}
			br->instruction_addr = vsaddr;
			//br->instruction_next_addr = vtaddr;
			br->branch_target = vtaddr;
			br->is_conditional = br_cond == 0;
			br->is_indirect = br_direct == 1;
			br->is_return = br_subType == 0;
			br->is_call = br_subType == 1;
			br->taken = actrual_taken;		

			return 1;
		} 
		else {
			return 0;
		}
		//*/
	};
	