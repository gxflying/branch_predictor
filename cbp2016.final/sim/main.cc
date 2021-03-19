///////////////////////////////////////////////////////////////////////
//  Copyright 2015 Samsung Austin Semiconductor, LLC.                //
///////////////////////////////////////////////////////////////////////

//Description : Main file for CBP2016 

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <map>
using namespace std;

#include "utils.h"
//#include "bt9.h"
#include "bt9_reader.h"
//#include "predictor.cc"
#include "predictor.h"

#include <ctime>
#include <time.h>

#define GETTIMEOFDAY mingw_gettimeofday

#define COUNTER     unsigned long long



void CheckHeartBeat(UINT64 numIter, UINT64 numMispred)
{
  UINT64 dotInterval=1000000;
  UINT64 lineInterval=30*dotInterval;

 UINT64 d1K   =1000;
 UINT64 d10K  =10000;
 UINT64 d100K =100000;
 UINT64 d1M   =1000000; 
 UINT64 d10M  =10000000;
 UINT64 d30M  =30000000;
 UINT64 d60M  =60000000;
 UINT64 d100M =100000000;
 UINT64 d300M =300000000;
 UINT64 d600M =600000000;
 UINT64 d1B   =1000000000;
 UINT64 d10B  =10000000000;


//  if(numIter % lineInterval == 0){ //prints line every 30 million branches
//    printf("\n");
//    fflush(stdout);
//  }
  if(numIter == d1K){ //prints MPKI after 100K branches
    printf("  MPKBr_1K         \t : %10.4f\n",   1000.0*(double)(numMispred)/(double)(numIter));   
    fflush(stdout);
  }

  if(numIter == d10K){ //prints MPKI after 100K branches
    printf("  MPKBr_10K         \t : %10.4f\n",   1000.0*(double)(numMispred)/(double)(numIter));   
    fflush(stdout);
  }
  
  if(numIter == d100K){ //prints MPKI after 100K branches
    printf("  MPKBr_100K         \t : %10.4f\n",   1000.0*(double)(numMispred)/(double)(numIter));   
    fflush(stdout);
  }
  if(numIter == d1M){
    printf("  MPKBr_1M         \t : %10.4f\n",   1000.0*(double)(numMispred)/(double)(numIter)); 
    fflush(stdout);
  }

  if(numIter == d10M){ //prints MPKI after 100K branches
    printf("  MPKBr_10M         \t : %10.4f\n",   1000.0*(double)(numMispred)/(double)(numIter));   
    fflush(stdout);
  }

  if(numIter == d30M){ //prints MPKI after 100K branches
    printf("  MPKBr_30M         \t : %10.4f\n",   1000.0*(double)(numMispred)/(double)(numIter));   
    fflush(stdout);
  }

  if(numIter == d60M){ //prints MPKI after 100K branches
    printf("  MPKBr_60M         \t : %10.4f\n",   1000.0*(double)(numMispred)/(double)(numIter));   
    fflush(stdout);
  }

  if(numIter == d100M){ //prints MPKI after 100K branches
    printf("  MPKBr_100M         \t : %10.4f\n",   1000.0*(double)(numMispred)/(double)(numIter));   
    fflush(stdout);
  }
  
  if(numIter == d300M){ //prints MPKI after 100K branches
    printf("  MPKBr_300M         \t : %10.4f\n",   1000.0*(double)(numMispred)/(double)(numIter));   
    fflush(stdout);
  }

  if(numIter == d600M){ //prints MPKI after 100K branches
    printf("  MPKBr_600M         \t : %10.4f\n",   1000.0*(double)(numMispred)/(double)(numIter));   
    fflush(stdout);
  }

  if(numIter == d1B){ //prints MPKI after 100K branches
    printf("  MPKBr_1B         \t : %10.4f\n",   1000.0*(double)(numMispred)/(double)(numIter));   
    fflush(stdout);
  }
  
  if(numIter == d10B){ //prints MPKI after 100K branches
    printf("  MPKBr_10B         \t : %10.4f\n",   1000.0*(double)(numMispred)/(double)(numIter));   
    fflush(stdout);
  }
 
};//void CheckHeartBeat

typedef struct br_type_ {
  bool is_condition;
  bool is_direct_jmp;
  bool is_call;
  bool is_ret;
  bool is_jmp;
  bool is_loop;
  br_type_() {
  is_condition = false;
  is_direct_jmp = false;
  is_call = false;
  is_ret = false;
  is_loop = false;
  };
} stBr_type;

typedef struct br_record_ {
  unsigned long long condition;
  unsigned long long uncondition;
  unsigned long long direct_jmp;
  unsigned long long indirect_jmp;
  unsigned long long call;
  unsigned long long ret;
  unsigned long long loop;
  br_record_() {
    condition = 0;
    uncondition = 0;
    direct_jmp = 0;
    indirect_jmp = 0;
    call = 0;
    ret = 0;
    loop = 0;
  };
} stBr_record;


OpType get_opType(const bt9::BrClass & br_class, const unsigned int Node_idx, stBr_type &brType, stBr_record & brRecord)
{
    OpType opType = OPTYPE_ERROR; 

    switch (br_class.type) {
      case  bt9::BrClass::Type::RET : {
        brType.is_ret = true;
        brRecord.ret += 1;
        break;
      }
      case  bt9::BrClass::Type::CALL : {
        brType.is_call = true;
        brRecord.call == 1;
        break;
      }
      case  bt9::BrClass::Type::JMP : {
        brType.is_jmp = true;
        break;
      }
    }
    switch (br_class.directness) {
      case  bt9::BrClass::Directness::INDIRECT : {
        brType.is_direct_jmp = false;
        brRecord.indirect_jmp += 1;
        break;
      }
      case  bt9::BrClass::Directness::DIRECT : {
        brType.is_direct_jmp = true;
        brRecord.direct_jmp += 1;
        break;
      }
    }    
   switch (br_class.conditionality) {
      case  bt9::BrClass::Conditionality::CONDITIONAL : {
        brType.is_condition = true;
        brRecord.condition += 1;
        break;
      }
      case  bt9::BrClass::Conditionality::UNCONDITIONAL : {
        brType.is_condition = false;
        brRecord.uncondition += 1;
        break;
      }
    }    


    if ((br_class.type == bt9::BrClass::Type::UNKNOWN) && Node_idx) { //only fault if it isn't the first node in the graph (fake branch)
      opType = OPTYPE_ERROR; //sanity check
    }
    else if (br_class.type == bt9::BrClass::Type::RET) {
      if (br_class.conditionality == bt9::BrClass::Conditionality::CONDITIONAL) {
        opType = OPTYPE_RET_COND;     
      } else if (br_class.conditionality == bt9::BrClass::Conditionality::UNCONDITIONAL) {
        opType = OPTYPE_RET_UNCOND;    
      } else {
        opType = OPTYPE_ERROR;
      }
    }
    else if (br_class.directness == bt9::BrClass::Directness::INDIRECT) {
      if (br_class.type == bt9::BrClass::Type::CALL) {
        if (br_class.conditionality == bt9::BrClass::Conditionality::CONDITIONAL) {
          opType = OPTYPE_CALL_INDIRECT_COND;
        } else if (br_class.conditionality == bt9::BrClass::Conditionality::UNCONDITIONAL) {
          opType = OPTYPE_CALL_INDIRECT_UNCOND;     
        } else {
          opType = OPTYPE_ERROR;
        }
      }
      else if (br_class.type == bt9::BrClass::Type::JMP) {
        if (br_class.conditionality == bt9::BrClass::Conditionality::CONDITIONAL) {
          opType = OPTYPE_JMP_INDIRECT_COND;     
        } else if (br_class.conditionality == bt9::BrClass::Conditionality::UNCONDITIONAL) {
          opType = OPTYPE_JMP_INDIRECT_UNCOND;  
        } else {
          opType = OPTYPE_ERROR;
        }
      }
      else {
        opType = OPTYPE_ERROR;
      }
    }
    else if (br_class.directness == bt9::BrClass::Directness::DIRECT) {
      if (br_class.type == bt9::BrClass::Type::CALL) {
        if (br_class.conditionality == bt9::BrClass::Conditionality::CONDITIONAL) {
          opType = OPTYPE_CALL_DIRECT_COND;
        }
        else if (br_class.conditionality == bt9::BrClass::Conditionality::UNCONDITIONAL) {
          opType = OPTYPE_CALL_DIRECT_UNCOND;
        }
        else {
          opType = OPTYPE_ERROR;
        }
      }
      else if (br_class.type == bt9::BrClass::Type::JMP) {
        if (br_class.conditionality == bt9::BrClass::Conditionality::CONDITIONAL) {
          opType = OPTYPE_JMP_DIRECT_COND;
        }
        else if (br_class.conditionality == bt9::BrClass::Conditionality::UNCONDITIONAL) {
          opType = OPTYPE_JMP_DIRECT_UNCOND;
        }
        else {
          opType = OPTYPE_ERROR;
        }
      }
      else {
        opType = OPTYPE_ERROR;
      }
    }
    else {
      opType = OPTYPE_ERROR;
    }

    return opType;
};




// usage: predictor <trace>

int main(int argc, char* argv[]){
  
  if (argc != 2) {
    printf("usage: %s <trace>\n", argv[0]);
    exit(-1);
  }
  
  ///////////////////////////////////////////////
  // Init variables
  ///////////////////////////////////////////////
    
    PREDICTOR  *brpred = new PREDICTOR();  // this instantiates the predictor code
  ///////////////////////////////////////////////
  // read each trace recrod, simulate until done
  ///////////////////////////////////////////////

    std::string trace_path;
    trace_path = argv[1];
    bt9::BT9Reader bt9_reader(trace_path);

    std::string key = "total_instruction_count:";
    std::string value;
    bt9_reader.header.getFieldValueStr(key, value);
    UINT64     total_instruction_counter = std::stoull(value, nullptr, 0);
    UINT64 current_instruction_counter = 0;
    key = "branch_instruction_count:";
    bt9_reader.header.getFieldValueStr(key, value);
    UINT64     branch_instruction_counter = std::stoull(value, nullptr, 0);
    UINT64     numMispred =0;  
//ver2    UINT64     numMispred_btbMISS =0;  
//ver2    UINT64     numMispred_btbANSF =0;  
//ver2    UINT64     numMispred_btbATSF =0;  
//ver2    UINT64     numMispred_btbDYN =0;  

    UINT64 cond_branch_instruction_counter=0;
//ver2     UINT64 btb_ansf_cond_branch_instruction_counter=0;
//ver2     UINT64 btb_atsf_cond_branch_instruction_counter=0;
//ver2     UINT64 btb_dyn_cond_branch_instruction_counter=0;
//ver2     UINT64 btb_miss_cond_branch_instruction_counter=0;
           UINT64 uncond_branch_instruction_counter=0;

//ver2    ///////////////////////////////////////////////
//ver2    // model simple branch marking structure
//ver2    ///////////////////////////////////////////////
//ver2    std::map<UINT64, UINT32> myBtb; 
//ver2    map<UINT64, UINT32>::iterator myBtbIterator;
//ver2
//ver2    myBtb.clear();
   
  ///////////////////////////////////////////////
  // read each trace record, simulate until done
  ///////////////////////////////////////////////
    timeval tv;
    GETTIMEOFDAY(&tv, 0);
    int64_t t0 =  (int64_t)tv.tv_sec * 1000000 + (int64_t)tv.tv_usec;
      OpType opType;
      UINT64 PC;
      bool branchTaken;
      UINT64 branchTarget;
      UINT64 numIter = 0;

      stBr_record brRecord;
      stBr_record brRecord_misp;

      for (auto it = bt9_reader.begin(); it != bt9_reader.end(); ++it) {
        //CheckHeartBeat(++numIter, numMispred); //Here numIter will be equal to number of branches read

        try {
          bt9::BrClass br_class = it->getSrcNode()->brClass();

//          bool dirDynamic = (it->getSrcNode()->brObservedTakenCnt() > 0) && (it->getSrcNode()->brObservedNotTakenCnt() > 0); //JD2_2_2016
//          bool dirNeverTkn = (it->getSrcNode()->brObservedTakenCnt() == 0) && (it->getSrcNode()->brObservedNotTakenCnt() > 0); //JD2_2_2016

//JD2_2_2016 break down branch instructions into all possible types
          stBr_type brType; 
          opType = get_opType(br_class, it->getSrcNode()->brNodeIndex(), brType, brRecord);
  
          PC = it->getSrcNode()->brVirtualAddr();

          branchTaken = it->getEdge()->isTakenPath();
          branchTarget = it->getEdge()->brVirtualTarget();

          if (opType == OPTYPE_ERROR) { 
            if (it->getSrcNode()->brNodeIndex()) { //only fault if it isn't the first node in the graph (fake branch)
              fprintf(stderr, "OPTYPE_ERROR\n");
              printf("OPTYPE_ERROR\n");
              exit(-1); //this should never happen, if it does please email CBP org chair.
            }
          }
          else if (br_class.conditionality == bt9::BrClass::Conditionality::CONDITIONAL) { //JD2_17_2016 call UpdatePredictor() for all branches that decode as conditional
            //printf("COND ");
            bool predDir = false;

            predDir = brpred->GetPrediction(PC);
            brpred->UpdatePredictor(PC, opType, branchTaken, predDir, branchTarget); 

            if(predDir != branchTaken){
              numMispred++; // update mispred stats brRecord_misp

            }
            cond_branch_instruction_counter++;

          }
          else if (br_class.conditionality == bt9::BrClass::Conditionality::UNCONDITIONAL) { // for predictors that want to track unconditional branches
            uncond_branch_instruction_counter++;
            brpred->TrackOtherInst(PC, opType, branchTaken, branchTarget);
          }
          else {
            fprintf(stderr, "CONDITIONALITY ERROR\n");
            printf("CONDITIONALITY ERROR\n");
            exit(-1); //this should never happen, if it does please email CBP org chair.
          }

/************************************************************************************************************/
        }
        catch (const std::out_of_range & ex) {
          std::cout << ex.what() << '\n';
          break;
        }
      
      } //for (auto it = bt9_reader.begin(); it != bt9_reader.end(); ++it)


    ///////////////////////////////////////////
    //print_stats
    ///////////////////////////////////////////

    //NOTE: competitors are judged solely on MISPRED_PER_1K_INST. The additional stats are just for tuning your predictors.
    GETTIMEOFDAY(&tv, 0);
    int64_t t1 =  (int64_t)tv.tv_sec * 1000000 + (int64_t)tv.tv_usec;
    printf("Time consume : %lld \r\n", t1 - t0);
      printf("  TRACE \t : %s" , trace_path.c_str()); 
      printf("  NUM_INSTRUCTIONS            \t : %10llu\n",   total_instruction_counter);
      printf("  NUM_BR                      \t : %10llu\n",   branch_instruction_counter-1); //JD2_2_2016 NOTE there is a dummy branch at the beginning of the trace...
      printf("  NUM_UNCOND_BR               \t : %10llu\n",   uncond_branch_instruction_counter);
      printf("  NUM_CONDITIONAL_BR          \t : %10llu\n",   cond_branch_instruction_counter);
      printf("  NUM_MISPREDICTIONS          \t : %10llu\n",   numMispred);
      printf("  MISPRED_PER_1K_INST         \t : %10.4f\n",   1000.0*(double)(numMispred)/(double)(total_instruction_counter));
      printf("\n");
}



