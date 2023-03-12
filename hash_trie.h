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
    // the initialized entry in each HashTrieNode are chained via `next` to facilitate enumeration
    HashTrieEntry *next;

    // by default it points to a TupleList
    HashTrieEntry();

    ~HashTrieEntry();

    // disable copy constructor, copy assignment operator
    HashTrieEntry(const HashTrieEntry &) = delete;

    HashTrieEntry &operator=(const HashTrieEntry &) = delete;

    // for debugging purposes
    friend std::ostream &operator<<(std::ostream &os, const HashTrieEntry &entry);

    std::ostream &print_with_indent(std::ostream &os, int num_tabs) const;
};

struct HashTrieNode {
    HashTrieEntry *hash_table;
    const unsigned long allocated_size;
    HashTrieNode *parent;
    // Head itself does not point to any data. Head->next is the first initialized entry.
    // If head->next == nullptr, then the entire hash trie node does not have any data inserted.
    HashTrieEntry *head;
    // next entry to be inserted should always be put at tail->next
    HashTrieEntry *tail;


    explicit HashTrieNode(unsigned long allocated_size_arg, HashTrieNode *parent_arg = nullptr);

    ~HashTrieNode();

    unsigned long hash(int attribute) const { return std::hash<int>()(attribute) % allocated_size; }

    HashTrieEntry &look_up(int attribute) const { return hash_table[hash(attribute)]; }

    void insert_tuple_at(TupleListNode *node, unsigned long index);

    // it also sets the node->parent as this (the current hash trie node)
    void replace_with_hash_trie_node_at(HashTrieNode *node, unsigned long index);

    unsigned long size() const { return num_initialized_entries; }

    // for debugging purposes
    friend std::ostream &operator<<(std::ostream &os, const HashTrieNode &node);

    std::ostream &print_with_indent(std::ostream &os, int num_tabs) const;

    // disable copy constructor, copy assignment operator
    HashTrieNode(const HashTrieNode &) = delete;

    HashTrieNode &operator=(const HashTrieNode &) = delete;
private:
    unsigned long num_initialized_entries;
};

#endif //TEAM02_HASH_TRIE_H
