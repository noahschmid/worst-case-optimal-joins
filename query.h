#ifndef TEAM02_QUERY_H
#define TEAM02_QUERY_H

#include <vector>
#include <iostream>
#include "hash_trie.h"
#include "joined_table_builder.h"

class JoinQuery {
public:
    const int num_tables;
    JoinedTableBuilder *joined_table_builder = nullptr;
    JoinQuery(const Table **tables, int num_tables, const std::vector<std::string>& attributes);
    Table *exec();
    ~JoinQuery();
private:
    void enumerate(int index);

    const Table **tables;
    HashTrieNode **hash_tries;
    HashTrieIterator **iterators;
    std::vector<std::string> attributes;
    // TODO: check memory leak here
    Table *results = nullptr;
    int num_attributes;
};

#endif //TEAM02_QUERY_H
