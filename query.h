#ifndef TEAM02_QUERY_H
#define TEAM02_QUERY_H

#include <vector>
#include <iostream>
#include "hash_trie.h"

class JoinQuery {
public:
    const int num_tables;
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
    Table *results;
    int num_attributes;
};


class JoinedTupleBuilder {
public:
    JoinedTupleBuilder(const Table **tables, int num_tables, const std::vector<std::string> & join_attributes);

    ~JoinedTupleBuilder() {
        free(occupied);
    }

    void add_tuple(int table_idx, Tuple *tuple);
    std::vector<std::vector<int>> build();

    std::vector<std::string> get_attributes() { return attributes; }

private:
    const Table **tables;
    int *start_idx;
    std::vector<std::vector<bool>> pick_attr;
    std::vector<std::vector<int>> data;
    bool *occupied;
    std::vector<std::string> join_attributes;
    std::vector<std::string> attributes;
    int num_tables;
    int total_attributes;
};

#endif //TEAM02_QUERY_H
