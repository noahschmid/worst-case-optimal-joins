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

//#define CALIBRATE
#define NUM_RUNS 5
#define CYCLES_REQUIRED 1e8
#define FREQUENCY 2.3e9

double rdtsc(Table* table, std::vector<std::string> attributes) {
    int i, num_runs;
    myInt64 cycles;
    myInt64 start;
    num_runs = NUM_RUNS;

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
            compute(A, B, n);
        }
        cycles = stop_tsc(start);

        if(cycles >= CYCLES_REQUIRED) break;

        num_runs *= 2;
    }
#endif

    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
        HashTrieNode *hash_trie = HashTrieNode::build(table, attributes);
    }

    cycles = stop_tsc(start)/num_runs;
    return (double) cycles;
}


int main(int argc, char **argv) {
    Table* table;
    std::vector<std::string> attributes;
    if (argc == 3){
        int num_columns = atoi(argv[1]);
        int num_rows = atoi(argv[2]);
        printf("columns=%d, rows=%d\n", num_columns, num_rows);

        for (int i=0;i<num_columns;i++){
            attributes.push_back("v");
        }
        

        table = new Table(num_columns);

        for(int i=0;i<num_rows;i++){
            std::vector<int> vec;
            for (int i=0;i<num_columns;i++){
                vec.push_back(i);
            }
            table->append_row(vec);
        }
    }
    else if(argc == 2){
        table = new Table(argv[1]);
        attributes = table->get_attributes();
        printf("%d\n", table->get_num_rows());
    }
    else{printf("usage: <num_columns> <num_rows> OR <path_to_tsv>\n"); return -1;}
    

    printf("starting benchmark...\n");
    double r = rdtsc(table, attributes);
    printf("benchmark finished\n");
    printf("RDTSC instruction:\n %lf cycles measured => %lf seconds, assuming frequency is %lf MHz. (change in source file if different)\n\n", r, r/(FREQUENCY), (FREQUENCY)/1e6);
    return 0;
}