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

#define NUM_RUNS 10
#define CYCLES_REQUIRED 1e7
#define FREQUENCY 2.8e9
#define WARMUP_RUNS 10

// to prevent dead code elimination
Table *base_table_ptr = nullptr;
long long difference = 0;

myInt64* rdtsc(const Table** tables, int num_tables, const std::vector<std::string>& attributes) {
    int i, num_runs;
    volatile myInt64 cycles;
    volatile myInt64 start;
    Table* tbl;
    myInt64 *results = (myInt64*)malloc(sizeof(myInt64)*NUM_RUNS);

    for (i = 0; i < WARMUP_RUNS; ++i) {
        start = start_tsc();
        tbl = JoinQuery(tables, num_tables, attributes).exec();
        cycles = stop_tsc(start);
        difference += tbl - base_table_ptr;
        delete tbl;
    }

    for (i = 0; i < NUM_RUNS; ++i) {
        start = start_tsc();
        tbl = JoinQuery(tables, num_tables, attributes).exec();
        results[i] = stop_tsc(start);
        difference += tbl - base_table_ptr;
        delete tbl;
    }

    return results;
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
    myInt64 *results = rdtsc(tables, num_tables, attributes);
    double mean = 0;
    double std = 0;

    /* calculate mean */
    for(int i = 0; i < NUM_RUNS; ++i) {
        mean += results[i];
    }

    mean /= (double)NUM_RUNS;

    /* calculate standard deviation */
    for(int i = 0; i < NUM_RUNS; ++i) {
        std += pow((results[i] - mean), 2);
    }

    std /= (double)NUM_RUNS;
    std = sqrt(std);

    printf("benchmark finished\n");
    printf("cycles: %lf duration: %lf stddev: %lf stddev time: %lf\n",
            mean, mean / (FREQUENCY), std, std / (FREQUENCY));
    // print the garbage value of the accumulator to prevent dead code elimination
    printf("accumulator: %lld\n", difference);

    // release memory
    for(int i=0;i<num_tables;i++){
        delete tables[i];
    }
    return 0;
}