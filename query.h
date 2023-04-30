#ifndef TEAM02_QUERY_H
#define TEAM02_QUERY_H

#include <vector>
#include <iostream>
#include "hash_trie.h"

class JoinQuery {
public:
    const int num_tables;
    JoinQuery(Table **tables, int num_tables, std::vector<std::string> attributes);
    Table *exec();

private:
    void enumerate(int index);

    Table **tables;
    HashTrieNode **hash_tries;
    HashTrieIterator **iterators;
    std::vector<std::string> attributes;
    Table *results;
    int num_attributes;
};


class JoinedTupleBuilder {
public:
    JoinedTupleBuilder(Table **tables, int num_tables, int num_attributes) : num_tables(num_tables), tables(tables), num_attributes(num_attributes) {
        occupied = (bool*)calloc(num_attributes, sizeof(bool));
        start_idx = (int*)malloc(sizeof(int)*num_tables);
        start_idx[0] = 0;

        for(int i = 1; i < num_tables; ++i) {
            start_idx[i] = start_idx[i-1] + tables[i-1]->get_num_attributes();
        }
    }

    ~JoinedTupleBuilder() {
        free(occupied);
    }

    void add_tuple(int table_idx, Tuple *tuple);
    std::vector<std::vector<int>> build();

private:
    Table **tables;
    int *start_idx;
    std::vector<std::vector<int>> data;
    bool *occupied;
    int num_attributes;
    int num_tables;
};

#endif //TEAM02_QUERY_H
