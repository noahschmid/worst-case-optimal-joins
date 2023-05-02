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
    JoinedTupleBuilder(Table **tables, int num_tables, std::vector<std::string> join_attributes) : num_tables(num_tables), tables(tables), join_attributes(join_attributes) {
        occupied = (bool*)calloc(join_attributes.size(), sizeof(bool));
        start_idx = (int*)malloc(sizeof(int)*num_tables);
        start_idx[0] = 0;

        total_attributes = 0;

        for(int i = 0; i < num_tables; ++i) {
            int num_attr = 0;
            std::vector<bool> pick;
            for(int j = 0; j < tables[i]->get_num_attributes(); ++j) {
                for(int k = 0; k < join_attributes.size(); ++k) {
                    if(!tables[i]->get_attributes()[j].compare(join_attributes[k])) {
                        if(!occupied[k]) {
                            occupied[k] = true;
                            pick.push_back(true);
                            num_attr++;
                        }

                        pick.push_back(false);
                        continue;
                    }

                    pick.push_back(true);
                    num_attr++;
                }
                pick_attr.push_back(pick); 
            }

            total_attributes += num_attr;

            if(i < num_tables - 1)
                start_idx[i+1] = start_idx[i] + num_attr;
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
    std::vector<std::vector<bool>> pick_attr;
    std::vector<std::vector<int>> data;
    bool *occupied;
    std::vector<std::string> join_attributes;
    int num_tables;
    int total_attributes;
};

#endif //TEAM02_QUERY_H
