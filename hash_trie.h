#ifndef TEAM02_HASH_TRIE_H
#define TEAM02_HASH_TRIE_H

#include "tuple_list.h"

struct HashTrieTable;

// Conceptually a HashTrieEntry is a pointer, either points to another hash table,
// or a linked list of tuples if this entry is a leaf node.
struct HashTrieEntry {
    bool child_is_table;
    union {
        HashTrieTable *child_table;
        TupleListNode *child_tuple_list_node;
    };

    // by default child_is_table is set to false
    HashTrieEntry();
};

struct HashTrieTable {
    HashTrieEntry *data;
    const int size;

    explicit HashTrieTable(int allocated_size);

    ~HashTrieTable();

    // disable copy constructor, copy assignment operator
    HashTrieTable(const HashTrieTable &) = delete;

    HashTrieTable &operator=(const HashTrieTable &) = delete;
};

#endif //TEAM02_HASH_TRIE_H
