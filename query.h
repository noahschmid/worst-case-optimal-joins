#ifndef TEAM02_QUERY_H
#define TEAM02_QUERY_H

#include <vector>
#include <iostream>
#include "hash_trie.h"

class JoinQuery {
public:
    const int num_tables;
    JoinQuery(Table **tables, int num_tables, std::vector<std::string> attributes);
    TupleList *exec();

private:
    void enumerate(int index);

    Table **tables;
    HashTrieNode **hash_tries;
    HashTrieIterator **iterators;
    std::vector<std::string> attributes;
    std::vector<TupleListNode*> results;
    int num_attributes;
};

#endif //TEAM02_QUERY_H
