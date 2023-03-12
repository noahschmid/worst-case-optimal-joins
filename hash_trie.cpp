#include "hash_trie.h"

using namespace std;

HashTrieEntry::HashTrieEntry() {
    points_to_tuple_list = true;
    tuple_list_ptr = nullptr;
    next = nullptr;
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
        if (entry.tuple_list_ptr) {
            os << *(entry.tuple_list_ptr);
        } else {
            os << "uninitialized entry";
        }
    } else {
        if (entry.hash_trie_node_ptr) {
            os << *(entry.hash_trie_node_ptr);
        } else {
            os << " uninitialized entry";
        }
    }
    return os;
}

std::ostream & HashTrieEntry::print_with_indent(std::ostream &os, int num_tabs) const {
    if (points_to_tuple_list) {
        if (tuple_list_ptr) {
            os << *tuple_list_ptr;
        } else {
            os << " uninitialized entry";
        }
    } else {
        if (hash_trie_node_ptr) {
            hash_trie_node_ptr->print_with_indent(os, num_tabs);
        } else {
            os << " uninitialized entry";
        }
    }
    return os;
}


HashTrieNode::HashTrieNode(unsigned long allocated_size_arg, HashTrieNode *parent_arg) :
        allocated_size(allocated_size_arg), parent(parent_arg) {
    hash_table = new HashTrieEntry[allocated_size_arg];
    head = new HashTrieEntry();
    tail = head;
}

HashTrieNode::~HashTrieNode() {
    delete[] hash_table;
    hash_table = nullptr;
    delete head;
    head = nullptr;
    tail = nullptr;
}

void HashTrieNode::insert_tuple_at(TupleListNode *node, unsigned long index) {
    HashTrieEntry &entry = hash_table[index];
    // TODO: once we are confident with implementation we can remove this check to speed up
    if (!entry.points_to_tuple_list) {
        std::cerr << "This entry already points to another hash trie node. Shouldn't append a tuple here.";
        exit(-1);
    }
    if (entry.tuple_list_ptr == nullptr) {
        entry.tuple_list_ptr = new TupleList();
        tail->next = &entry;
        tail = tail->next;
        num_initialized_entries ++;
    }
    entry.tuple_list_ptr->append(node);
}

void HashTrieNode::replace_with_hash_trie_node_at(HashTrieNode *node, unsigned long index) {
    HashTrieEntry &entry = hash_table[index];
    // TODO: once we are confident with implementation we can remove this check to speed up
    if (!entry.points_to_tuple_list) {
        std::cerr << "This entry already points to another hash trie node. Shouldn't override the entry.";
        exit(-1);
    }
    // we should only insert hash trie nodes at the places where we already placed the tuple list.
    // so no need to update tail.
    entry.points_to_tuple_list = false;
    delete entry.tuple_list_ptr;
    entry.hash_trie_node_ptr = node;
    node->parent = this;
}

std::ostream &operator<<(std::ostream &os, const HashTrieNode &node) {
    return node.print_with_indent(os, 0);
}


std::ostream & HashTrieNode::print_with_indent(std::ostream &os, int num_tabs) const {
    for (int i = 0; i < allocated_size; i++) {
        os << "at index " << i << ": " << std::endl;
        for (int j = 0; j < num_tabs + 1; j++) {
            os << '\t';
        }
        hash_table[i].print_with_indent(os, num_tabs+1);
        if (i != allocated_size - 1) {
            os << endl;
            for (int j = 0; j < num_tabs; j++) {
                os << '\t';
            }
        }
    }
    return os;
}
