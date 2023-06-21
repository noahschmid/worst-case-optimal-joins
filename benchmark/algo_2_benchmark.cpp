#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __x86_64__

#include "tsc_x86.h"

#endif

#include "../database.h"
#include "../hash_trie.h"
#include "../query.h"
#include "../table.h"
#include "../tuple_list.h"

using namespace std;

#define NUM_RUNS 10
#define CYCLES_REQUIRED 1e7
#define FREQUENCY 2.8e9
#define WARMUP_RUNS 10

// to prevent dead code elimination
HashTrieNode *base_hash_trie = nullptr;
long long difference = 0;

myInt64* rdtsc(Table *table, std::vector<std::string> attributes) {
    int i;
    volatile myInt64 cycles;
    volatile myInt64 start;
    HashTrieNode *tmp_hash_trie = nullptr;
    myInt64 *results = (myInt64*)malloc(sizeof(myInt64)*NUM_RUNS);

    for (i = 0; i < WARMUP_RUNS; ++i) {
        start = start_tsc();
        tmp_hash_trie = HashTrieNode::build(table, attributes);
        cycles = stop_tsc(start);
        difference += tmp_hash_trie - base_hash_trie;
        delete tmp_hash_trie;
    }

    for (i = 0; i < NUM_RUNS; ++i) {
        start = start_tsc();
        tmp_hash_trie = HashTrieNode::build(table, attributes);
        results[i] = stop_tsc(start);
        difference += tmp_hash_trie - base_hash_trie;
        delete tmp_hash_trie;
    }

    return results;
}

int main(int argc, char **argv) {

    std::string path_to_tsv;
    std::string table_name;

    Table *table;
    std::vector<std::string> attributes;

    if (argc == 3) {
        int num_columns = atoi(argv[1]);
        int num_rows = atoi(argv[2]);
        printf("columns=%d, rows=%d\n", num_columns, num_rows);

        for (int i = 0; i < num_columns; i++) {
            attributes.push_back("v");
        }

        table = new Table("bench_table", attributes);

        for (int i = 0; i < num_rows; i++) {
            std::vector<int> vec;
            for (int j = 0; j < num_columns; j++) {
                vec.push_back(j);
            }
            table->append_row(vec);
        }
    } else if (argc == 2) {
        table = new Table(argv[1], "bench_table");
        attributes = table->get_attributes();
        printf("%d\n", table->get_num_rows());
    } else {
        printf("usage: <num_columns> <num_rows> OR <path_to_tsv>\n");
        return -1;
    }

    printf("starting benchmark...\n");
    myInt64 *results = rdtsc(table, attributes);
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

    delete table;
    return 0;
}