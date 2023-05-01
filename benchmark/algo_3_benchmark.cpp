#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#ifdef __x86_64__
    #include "tsc_x86.h"
#endif

#include "../table.h"
#include "../tuple_list.h"
#include "../hash_trie.h"
#include "../query.h"
using namespace std;

#define CALIBRATE
#define NUM_RUNS 5
#define CYCLES_REQUIRED 1e10
#define FREQUENCY 2.3e9

double rdtsc(JoinQuery* query) {
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
#ifdef CALIBRATE
    while(num_runs < (1 << 14)) {
        start = start_tsc();
        for (i = 0; i < num_runs; ++i) {
            tbl = query->exec();
        }
        cycles = stop_tsc(start);

        if(cycles >= CYCLES_REQUIRED) break;

        num_runs *= 2;
    }
#endif

    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
        tbl = query->exec();
    }

    cycles = stop_tsc(start)/num_runs;
    return (double) cycles;
}

double c_clock(JoinQuery* query) {
    int i, num_runs;
    double cycles;
    clock_t start, end;
    Table* tbl;

    num_runs = NUM_RUNS;
#ifdef CALIBRATE
    while(num_runs < (1 << 14)) {
        start = clock();
        for (i = 0; i < num_runs; ++i) {
            tbl = query->exec();
        }
        end = clock();

        cycles = (double)(end-start);

        // Same as in c_clock: CYCLES_REQUIRED should be expressed accordingly to the order of magnitude of CLOCKS_PER_SEC
        if(cycles >= CYCLES_REQUIRED/(FREQUENCY/CLOCKS_PER_SEC)) break;

        num_runs *= 2;
    }
#endif
    start = clock();
    for(i=0; i<num_runs; ++i) { 
        tbl = query->exec();
    }
    end = clock();

    return (double)(end-start)/num_runs;
}


int main(int argc, char **argv) {
    if(argc <= 1){
        printf("usage: <NUM_TABLES> <PATH_TO_TABLE_1> ... <PATH_TO_TABLE_n> \n"); return -1;
    }
    int num_tables = atoi(argv[1]);
    Table* tables[argc-1];
    std::vector<std::string> attributes;
    for(int i=2;i<num_tables+2;i++){
        tables[i-2] = new Table(argv[i], argv[i]);
    }

    for(int i=num_tables+2;i<argc;i++){
        attributes.push_back(argv[i]);
    }
    
    JoinQuery query(tables, num_tables, attributes);


    printf("starting benchmark...\n");
    double r = rdtsc(&query);
    double c = c_clock(&query);
    printf("benchmark finished\n");
    printf("RDTSC instruction:\n %lf cycles measured => %lf seconds, assuming frequency is %lf MHz. (change in source file if different)\n\n", r, r/(FREQUENCY), (FREQUENCY)/1e6);
    printf("C clock() function:\n %lf cycles measured. On some systems, this number seems to be actually computed from a timer in seconds then transformed into clock ticks using the variable CLOCKS_PER_SEC. Unfortunately, it appears that CLOCKS_PER_SEC is sometimes set improperly. (According to this variable, your computer should be running at %lf MHz). In any case, dividing by this value should give a correct timing: %lf seconds. \n\n",c, (double) CLOCKS_PER_SEC/1e6, c/CLOCKS_PER_SEC);
    return 0;
}