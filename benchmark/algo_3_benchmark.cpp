#include <stdlib.h>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __x86_64__
    #include "tsc_x86.h"
#endif

#include "../table.h"
#include "../tuple_list.h"
#include "../hash_trie.h"
#include "../query.h"
using namespace std;

#define CALIBRATE
#define NUM_RUNS 16
#define CYCLES_REQUIRED 1e7
#define FREQUENCY 2.8e9

// to prevent dead code elimination
Table *base_table_ptr = nullptr;
long long difference = 0;

double rdtsc(const Table** tables, int num_tables, const std::vector<std::string>& attributes) {
    int i, num_runs;
    myInt64 cycles;
    myInt64 start;
    num_runs = NUM_RUNS;
    Table* tbl;
    JoinQuery query (tables, num_tables, attributes);
    /*
     * The CPUID instruction serializes the pipeline.
     * Using it, we can create execution barriers around the code we want to time.
     * The calibrate section is used to make the computation large enough so as to
     * avoid measurements bias due to the timing overhead.
     */
#ifdef CALIBRATE
    while(num_runs < (1 << 14)) {
        start = start_tsc();
        for (i = 0; i < num_runs; ++i) {
            // between executions quer.exec() will delete the previous result
            tbl = query.exec();
            difference += tbl - base_table_ptr;
        }
        cycles = stop_tsc(start);

        if(cycles >= CYCLES_REQUIRED) break;

        num_runs *= 2;
    }
#endif
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
        tbl = query.exec();
        difference += tbl - base_table_ptr;
    }

    cycles = stop_tsc(start)/num_runs;
    delete tbl;
    return (double) cycles;
}

double c_clock(const Table** tables, int num_tables, const std::vector<std::string> & attributes) {
    int i, num_runs;
    myInt64 cycles;
    myInt64 start;
    num_runs = NUM_RUNS;
    Table* tbl;
    /* 
     * The CPUID instruction serializes the pipeline.
     * Using it, we can create execution barriers around the code we want to time.
     * The calibrate section is used to make the computation large enough so as to 
     * avoid measurements bias due to the timing overhead.
     */
    JoinQuery query(tables, num_tables, attributes);

#ifdef CALIBRATE
    while(num_runs < (1 << 14)) {
        start = start_tsc();
        for (i = 0; i < num_runs; ++i) {
            tbl = query.exec();
            difference += tbl - base_table_ptr;
        }
        cycles = stop_tsc(start);
        if(cycles >= CYCLES_REQUIRED) break;

        num_runs *= 2;
    }
#endif
    
    start = clock();
    for (i = 0; i < num_runs; ++i) {
        tbl = query.exec();
        difference += tbl - base_table_ptr;
    }

    int stop = clock();
    delete tbl;
    return (double) (stop-start)/num_runs;
}


int main(int argc, char **argv) {
    if(argc <= 1){
        printf("usage: <NUM_TABLES> <PATH_TO_TABLE_1> ... <PATH_TO_TABLE_n> <COL_TO_JOIN_1> ... <COL_TO_JOIN_N>\n"); return -1;
    }
    int num_tables = atoi(argv[1]);
    const Table* tables[num_tables];
    std::vector<std::string> table_path;
    std::vector<std::string> attributes;

    for(int i=num_tables+2;i<argc;i++){
        attributes.push_back(argv[i]);
    }

    for(int i=2;i<num_tables+2;i++){
        tables[i-2] = new Table(argv[i], argv[i]);
        table_path.push_back(argv[i]);
    }


    printf("starting benchmark...\n");
    double r = rdtsc(tables, num_tables, attributes);
    double c = c_clock(tables, num_tables, attributes);
    printf("benchmark finished\n");
    printf("RDTSC instruction:\n %lf cycles measured => %lf seconds, assuming "
            "frequency is %lf MHz. (change in source file if different)\n\n",
            r, r / (FREQUENCY), (FREQUENCY) / 1e6);
    printf("C clock() function:\n %lf cycles measured. On some systems, this "
            "number seems to be actually computed from a timer in seconds then "
            "transformed into clock ticks using the variable CLOCKS_PER_SEC. "
            "Unfortunately, it appears that CLOCKS_PER_SEC is sometimes set "
            "improperly. (According to this variable, your computer should be "
            "running at %lf MHz). In any case, dividing by this value should give "
            "a correct timing: %lf seconds. \n\n",
            c, (double)CLOCKS_PER_SEC / 1e6, c / CLOCKS_PER_SEC);
    // print the garbage value of the accumulator to prevent dead code elimination
    printf("accumulator: %lld\n", difference);

    // release memory
    for(int i=0;i<num_tables;i++){
        delete tables[i];
    }
    return 0;
}