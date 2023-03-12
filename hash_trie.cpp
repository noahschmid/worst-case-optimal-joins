#include "hash_trie.h"

HashTrieEntry::HashTrieEntry() {
    child_is_table = false;
    child_tuple_list_node = nullptr;
}

HashTrieTable::HashTrieTable(int allocated_size) : size(allocated_size) {
    data = new HashTrieEntry[allocated_size];
}

HashTrieTable::~HashTrieTable() {
    delete[] data;
    data = nullptr;
}

