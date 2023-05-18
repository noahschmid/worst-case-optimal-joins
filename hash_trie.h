#ifndef TEAM02_HASH_TRIE_H
#define TEAM02_HASH_TRIE_H

#include "tuple_list.h"
#include <iostream>
#include <math.h>

struct HashTrieNode;

// Conceptually a HashTrieEntry is a pointer, either points to another HashTrieNode,
// or a TupleList if this entry is a leaf node.
// WARNING: they should only point to dynamically created variables, e.g. created via new.
struct HashTrieEntry {
    bool points_to_tuple_list;

    uint64_t hash;

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

    long parent_hash;
    int shift;

    explicit HashTrieNode(unsigned long allocated_size_arg, int shift, HashTrieNode *parent_arg = nullptr);

    ~HashTrieNode();

    unsigned long calc_hash(int attribute) const { return std::hash<uint64_t>()(attribute); }

    HashTrieEntry &look_up(int attribute) const { return hash_table[calc_hash(attribute)]; }

    void insert_tuple_at(uint64_t hash, Tuple *node);

    // it also sets the node->parent as this (the current hash trie node)
    void replace_with_hash_trie_node_at(HashTrieNode *node, unsigned long index);

    unsigned long size() const { return num_initialized_entries; }

    static HashTrieNode* build(const Table *table, const std::vector<std::string>& attributes);

    // for debugging purposes
    friend std::ostream &operator<<(std::ostream &os, const HashTrieNode &node);

    std::ostream &print_with_indent(std::ostream &os, int num_tabs) const;

    // disable copy constructor, copy assignment operator
    HashTrieNode(const HashTrieNode &) = delete;

    HashTrieNode &operator=(const HashTrieNode &) = delete;
private:
    static HashTrieEntry* build(int i, int *indeces, int size, TupleList *L);
    unsigned long num_initialized_entries;
};

class HashTrieIterator {
public:
    HashTrieIterator(HashTrieNode *node, std::string name);

    void up();
    void down();
    bool next();
    bool lookup(uint64_t hash);
    long get_hash() { return hash; };
    int get_size() { return size; };
    TupleList *get_tuples() { return tuples; };

    friend std::ostream &operator<<(std::ostream &os, const HashTrieIterator &it);
    HashTrieNode *cursor;
    std::string name;
private:
    TupleList *tuples;
    
    uint64_t hash;
    int previous_bucket;
    int bucket;
    int size;  
};


#endif //TEAM02_HASH_TRIE_H
