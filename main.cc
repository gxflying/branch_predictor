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

// usage: predictor <trace>
int
main(int argc, char* argv[])
{
    using namespace std;

    if (3 != argc) {
        printf("usage: %s <trace> ugz\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    printf("sizeof (stTraceMeta) : %d\r\n", sizeof(stTraceMeta));
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
        // ************************************************************
        // Competing predictors must have the following methods:
        // ************************************************************

        // get_prediction() returns the prediction your predictor would like to make
        bool predicted_taken = predictor.get_prediction(&br, NULL);

        // predict_branch() tells the trace reader how you have predicted the branch
        bool actual_taken    = br.taken;
        if( predicted_taken != actual_taken) 
        {
            mispredicts++;
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

    printf("total instructions : %lld   mispredict : %lld\r\n", instr_cnt, mispredicts);
    printf("mispredict ratio : %f\r\n", 1.0*mispredicts/instr_cnt);

}

/*
// usage: predictor <trace>
int
main(int argc, char* argv[])
{
    using namespace std;

    if (2 != argc) {
        printf("usage: %s <trace>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    cbp_trace_reader_c cbptr = cbp_trace_reader_c(argv[1]);
    branch_record_c1 br;

    // read the trace, one branch at a time, placing the branch info in br
    while (cbptr.get_branch_record(&br)) {

        // ************************************************************
        // Competing predictors must have the following methods:
        // ************************************************************

        // get_prediction() returns the prediction your predictor would like to make
        bool predicted_taken = predictor.get_prediction(&br, cbptr.osptr);

        // predict_branch() tells the trace reader how you have predicted the branch
        bool actual_taken    = cbptr.predict_branch(predicted_taken);
            
        // finally, update_predictor() is used to update your predictor with the
        // correct branch result
        predictor.update_predictor(&br, cbptr.osptr, actual_taken);

    }

 
}
*/



