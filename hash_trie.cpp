#include "hash_trie.h"

using namespace std;

HashTrieEntry::HashTrieEntry() {
    points_to_tuple_list = true;
    tuple_list_ptr = new TupleList;
}

HashTrieEntry::~HashTrieEntry() {
    if (points_to_tuple_list) {
        delete tuple_list_ptr;
        tuple_list_ptr = nullptr;
    } else {
        delete hash_trie_node_ptr;
        hash_trie_node_ptr = nullptr;
    }
}

std::ostream &operator<<(std::ostream &os, const HashTrieEntry &entry) {
    if (entry.points_to_tuple_list) {
        os << *(entry.tuple_list_ptr);
    } else {
        os << *(entry.hash_trie_node_ptr);
    }
    return os;
}

std::ostream &print_with_indent(std::ostream &os, const HashTrieEntry &entry, int num_tabs) {
    if (entry.points_to_tuple_list) {
        os << *entry.tuple_list_ptr;
    } else {
        print_with_indent(os, *entry.hash_trie_node_ptr, num_tabs);
    }
    return os;
}


HashTrieNode::HashTrieNode(unsigned long allocated_size, HashTrieNode *parent_arg) :
        size(allocated_size), parent(parent_arg) {
    hash_table = new HashTrieEntry[allocated_size];
}

HashTrieNode::~HashTrieNode() {
    delete[] hash_table;
    hash_table = nullptr;
}

void HashTrieNode::insert_tuple_at(TupleListNode *node, int index) {
    HashTrieEntry &entry = hash_table[index];
    // TODO: once we are confident with implementation we can remove this check to speed up
    if (!entry.points_to_tuple_list) {
        std::cerr << "This entry already points to another hash trie node. Shouldn't append a tuple here.";
        exit(-1);
    }

    entry.tuple_list_ptr->append(node);
}

void HashTrieNode::insert_hash_trie_node_at(HashTrieNode *node, int index) {
    HashTrieEntry &entry = hash_table[index];
    // TODO: once we are confident with implementation we can remove this check to speed up
    if (!entry.points_to_tuple_list) {
        std::cerr << "This entry already points to another hash trie node. Shouldn't override the entry.";
        exit(-1);
    }

    entry.points_to_tuple_list = false;
    entry.hash_trie_node_ptr = node;
    node->parent = this;
}

std::ostream &operator<<(std::ostream &os, const HashTrieNode &node) {
    return print_with_indent(os, node, 0);
}


std::ostream &print_with_indent(std::ostream &os, const HashTrieNode &node, int num_tabs) {
    for (int i = 0; i < node.size; i++) {
        os << "at index " << i << ": " << std::endl;
        for (int j = 0; j < num_tabs + 1; j++) {
            os << '\t';
        }
        print_with_indent(os, node.hash_table[i], num_tabs + 1);
        if (i != node.size - 1) {
            os << endl;
            for (int j = 0; j < num_tabs; j++) {
                os << '\t';
            }
        }
    }
    return os;
}
