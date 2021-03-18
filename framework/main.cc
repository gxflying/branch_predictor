/* Author: Chris Wilkerson;   Created: Thu Aug 12 16:19:58 PDT 2004
 * Description: Branch predictor driver.
*/

#include <cstdio>
#include <cstdlib>
#include "tread.h"
#include "my_trace_reader.h"

#include <ctime>
#include <time.h>

#define GETTIMEOFDAY mingw_gettimeofday


// include and define the predictor
#include "predictor.h"
PREDICTOR predictor;


#if 1
// usage: predictor <trace>
int
main(int argc, char* argv[])
{
    using namespace std;

    if (3 != argc) {
        printf("usage: %s <trace> ugz\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    printf("sizeof (stTraceMeta) : %d 2222 \r\n", sizeof(stTraceMeta));
    //return 0;
    branch_record_c1 br;
	CMyGzTraceReader treader(argv[1], argv[2]);
    treader.preoperation();  

    long long instr_cnt = 0;
    long long mispredicts = 0;
    bool reg1[3] = {false, false};
    // read the trace, one branch at a time, placing the branch info in br

    time_t rawtime;
    struct tm *ptminfo;
    time(&rawtime);
    ptminfo = localtime(&rawtime);
    printf("current: %02d-%02d-%02d %02d:%02d:%02d\n",
            ptminfo->tm_year + 1900, ptminfo->tm_mon + 1, ptminfo->tm_mday,
            ptminfo->tm_hour, ptminfo->tm_min, ptminfo->tm_sec);

    timeval tv;
    GETTIMEOFDAY(&tv, 0);
    int64_t t0 =  (int64_t)tv.tv_sec * 1000000 + (int64_t)tv.tv_usec;


    while (treader.getLine(&br)) {
        instr_cnt++;
        //printf("instr_cnt : %d\n", instr_cnt);
        // ************************************************************
        // Competing predictors must have the following methods:
        // ************************************************************
        bool actual_taken    = br.taken;
        // predict_branch() tells the trace reader how you have predicted the branch

        // get_prediction() returns the prediction your predictor would like to make
        bool predicted_taken = predictor.get_prediction(&br, NULL);

        if (br.is_conditional) {
            if( predicted_taken != actual_taken) 
            {
                mispredicts++;
            }
        } 
        reg1[0] =reg1[1];
        reg1[1] =reg1[2];
        reg1[2] = actual_taken;

        bool update_dirction = reg1[2];
        //bool update_dirction = actual_taken;
        // finally, update_predictor() is used to update your predictor with the
        // correct branch result
        predictor.update_predictor(&br, NULL, update_dirction);
    }

    GETTIMEOFDAY(&tv, 0);
    int64_t t1 =  (int64_t)tv.tv_sec * 1000000 + (int64_t)tv.tv_usec;
    printf("Time consume : %lld \r\n", t1 - t0);
    printf("g_total_instr_count : %lld\n", treader.g_total_instr_count);
    printf("total instructions : %lld   mispredict : %lld\r\n", instr_cnt, mispredicts);
    printf("mispredict ratio : %f\r\n", 1000.0*mispredicts/treader.g_total_instr_count);

}
#endif

#if 0
// usage: predictor <trace>
int
main(int argc, char* argv[])
{
    using namespace std;

    if (3 != argc) {
        printf("usage: %s <trace> ugz\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    printf("sizeof (stTraceMeta) : %d 2222 \r\n", sizeof(stTraceMeta));
    //return 0;
    branch_record_c1 br;
	CMyGzTraceReader treader(argv[1], argv[2]);
    treader.preoperation();  

    long long instr_cnt = 0;
    long long mispredicts = 0;
    bool reg1[3] = {false, false};
    // read the trace, one branch at a time, placing the branch info in br

    time_t rawtime;
    struct tm *ptminfo;
    time(&rawtime);
    ptminfo = localtime(&rawtime);
    printf("current: %02d-%02d-%02d %02d:%02d:%02d\n",
            ptminfo->tm_year + 1900, ptminfo->tm_mon + 1, ptminfo->tm_mday,
            ptminfo->tm_hour, ptminfo->tm_min, ptminfo->tm_sec);

    timeval tv;
    GETTIMEOFDAY(&tv, 0);
    int64_t t0 =  (int64_t)tv.tv_sec * 1000000 + (int64_t)tv.tv_usec;


      OpType opType;
      UINT64 PC;
      bool branchTaken;
      UINT64 branchTarget;
      UINT64 numIter = 0;

    while (treader.getLine(&br)) {
        instr_cnt++;
        //printf("instr_cnt : %d\n", instr_cnt);
        // ************************************************************
        // Competing predictors must have the following methods:
        // ************************************************************
        bool actual_taken    = br.taken;
        // predict_branch() tells the trace reader how you have predicted the branch

        // get_prediction() returns the prediction your predictor would like to make
        //bool predicted_taken = predictor.get_prediction(&br, NULL);

        PREDICTOR * brpred = & predictor;

          opType = OPTYPE_ERROR; 
  
          PC = br.instruction_addr;

          branchTaken = br.taken;
          branchTarget = br.branch_target;

          if (br.is_conditional) { //JD2_17_2016 call UpdatePredictor() for all branches that decode as conditional
            //printf("COND ");
            bool predDir = false;

            predDir = brpred->GetPrediction(PC);
            brpred->UpdatePredictor(PC, opType, branchTaken, predDir, branchTarget); 

            if(predDir != branchTaken){
              mispredicts++; // update mispred stats

            }
          }
          else {
            brpred->TrackOtherInst(PC, opType, branchTaken, branchTarget);
          } 

    }

    GETTIMEOFDAY(&tv, 0);
    int64_t t1 =  (int64_t)tv.tv_sec * 1000000 + (int64_t)tv.tv_usec;
    printf("Time consume : %lld \r\n", t1 - t0);
    printf("g_total_instr_count : %lld\n", treader.g_total_instr_count);
    printf("total instructions : %lld   mispredict : %lld\r\n", instr_cnt, mispredicts);
    printf("mispredict ratio : %f\r\n", 1000.0*mispredicts/treader.g_total_instr_count);

}
#endif