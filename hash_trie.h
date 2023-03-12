#ifndef TEAM02_HASH_TRIE_H
#define TEAM02_HASH_TRIE_H

#include "tuple_list.h"
#include <iostream>

struct HashTrieNode;

// Conceptually a HashTrieEntry is a pointer, either points to another HashTrieNode,
// or a TupleList if this entry is a leaf node.
// WARNING: they should only point to dynamically created variables, e.g. created via new.
struct HashTrieEntry {
    bool points_to_tuple_list;
    union {
        HashTrieNode *hash_trie_node_ptr;
        TupleList *tuple_list_ptr;
    };

    // by default it points to a TupleList
    HashTrieEntry();

    ~HashTrieEntry();

    // disable copy constructor, copy assignment operator
    HashTrieEntry(const HashTrieEntry &) = delete;

    HashTrieEntry &operator=(const HashTrieEntry &) = delete;

    // for debugging purposes
    friend std::ostream &operator<<(std::ostream &os, const HashTrieEntry &entry);

    friend std::ostream &print_with_indent(std::ostream &os, const HashTrieEntry &entry, int num_tabs);
};

struct HashTrieNode {
    HashTrieEntry *hash_table;
    const unsigned long size;
    HashTrieNode *parent;

    explicit HashTrieNode(unsigned long allocated_size, HashTrieNode *parent_arg = nullptr);

    ~HashTrieNode();

    // disable copy constructor, copy assignment operator
    HashTrieNode(const HashTrieNode &) = delete;

    HashTrieNode &operator=(const HashTrieNode &) = delete;

    unsigned long hash(int attribute) const { return std::hash<int>()(attribute) % size; }

    HashTrieEntry &look_up(int attribute) const { return hash_table[hash(attribute)]; }

    void insert_tuple_at(TupleListNode *node, int index);

    void insert_hash_trie_node_at(HashTrieNode *node, int index);

    // for debugging purposes
    friend std::ostream &operator<<(std::ostream &os, const HashTrieNode &node);

    friend std::ostream &print_with_indent(std::ostream &os, const HashTrieNode &node, int num_tabs);
};

#endif //TEAM02_HASH_TRIE_H
