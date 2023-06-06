#include "hash_trie.h"
#include <assert.h>

using namespace std;

HashTrieEntry::HashTrieEntry() {
    points_to_tuple_list = true;
    tuple_list_ptr = nullptr;
    next = nullptr;
    hash = 0;
    hash_trie_node_ptr = nullptr;
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
            os << "[]";
        }
    } else {
        if (entry.hash_trie_node_ptr) {
            os << *(entry.hash_trie_node_ptr);
        } else {
            os << "nil";
        }
    }
    return os;
}

std::ostream & HashTrieEntry::print_with_indent(std::ostream &os, int num_tabs) const {
    if (points_to_tuple_list) {
        if (tuple_list_ptr) {
            os << *tuple_list_ptr;
        } else {
            os << "[nil]";
        }
    } else {
        if (hash_trie_node_ptr) {
            hash_trie_node_ptr->print_with_indent(os, num_tabs);
        } else {
            os << "nil";
        }
    }
    return os;
}


HashTrieNode::HashTrieNode(unsigned long allocated_size_arg, int shift, HashTrieNode *parent_arg) :
        allocated_size(allocated_size_arg), parent(parent_arg), shift(64 - shift) {
    hash_table = (HashTrieEntry**)calloc(allocated_size_arg, sizeof(HashTrieEntry*));
    head = new HashTrieEntry();
    tail = head;
}

HashTrieNode::~HashTrieNode() {
    free(hash_table);
    hash_table = nullptr;
    delete head;
    head = nullptr;
    tail = nullptr;
}

void HashTrieNode::insert_tuple_at(int hash, Tuple *node) {
    int index = hash >> shift;

    // if there is no entry yet, create it
    if(!hash_table[index]) {
        HashTrieEntry *entry = new HashTrieEntry();
        hash_table[index] = entry;
        hash_table[index]->hash = hash;
    } else {
        // in case of hash collision find next free spot
        index %= allocated_size;
        while(hash_table[index]->hash != hash) {
            index++;
            if(index >= allocated_size)
                index=0;

            // if there is an unallocated entry, we have found a free spot
            if(!hash_table[index]) {
                HashTrieEntry *entry = new HashTrieEntry();
                hash_table[index] = entry;
                hash_table[index]->hash = hash;
                break;
            }
        }
    }

    hash_table[index]->hash = hash;
    HashTrieEntry *entry = hash_table[index];
    
    if (entry->tuple_list_ptr == nullptr) {
        entry->tuple_list_ptr = new TupleList();
        tail->next = entry;
        tail = tail->next;
        num_initialized_entries ++;
    }
    entry->tuple_list_ptr->append(node);
}

std::ostream &operator<<(std::ostream &os, const HashTrieNode &node) {
    return node.print_with_indent(os, 0) << endl << "end\n";
}


std::ostream & HashTrieNode::print_with_indent(std::ostream &os, int num_tabs) const {
    for (int i = 0; i < allocated_size; i++) {
        if(!hash_table[i])
            continue;
        os << "index " << i << " [" << hash_table[i]->hash << "]: " << std::endl;
        for (int j = 0; j < num_tabs + 1; j++) {
            os << "   ";
        }
        hash_table[i]->print_with_indent(os, num_tabs+1);
        if (i != allocated_size - 1) {
            os << endl;
            for (int j = 0; j < num_tabs; j++) {
                os << "   ";
            }
        }
    }
    return os;
}

HashTrieNode* HashTrieNode::build(int idx, const int *indices, int len, TupleList *L) {
    int p = ceil(log2(1.25f*L->length()));
    unsigned long size = pow(2, p);
    HashTrieNode *M = new HashTrieNode(size, p);
    
    while(!L->empty()) {
        Tuple *t = L->pop_left();

        // OPTIMIZATION: remove call to calc_hash since it simply returns the value anyway
        M->insert_tuple_at(t->data[indices[idx]], t);
    }

    if (idx >= len-1) {
        return M;
    }
    ++idx;
    HashTrieEntry *curr = M->head;
    while (curr->next) {
        curr = curr->next;
        HashTrieNode *M_next = build(idx, indices, len, curr->tuple_list_ptr);
        delete curr->tuple_list_ptr;
        curr->tuple_list_ptr = nullptr;
        curr->points_to_tuple_list = false;
        curr->hash_trie_node_ptr = M_next;
        curr->hash_trie_node_ptr->parent = M;
    }

    return M;
}

HashTrieNode* HashTrieNode::build(const Table *table, const std::vector<std::string>& attributes) {
    std::vector<int> indices;

    for(const std::string& s1 : attributes) {
        int i = 0;
        for(const std::string& s2 : table->attributes) {
            if(s1 == s2) {
                indices.push_back(i);
                break;
            }
            ++i;
        }
    }

    TupleList L = TupleList(*table);
    HashTrieNode *node = build(0, indices.data(), indices.size(), &L);

    return node;
}


HashTrieIterator::HashTrieIterator(HashTrieNode *node, std::string name) : name(std::move(name)) {
    entry = node->head->next;
    prev_entries = std::stack<HashTrieEntry *>();

    cursor = node;
    tuples = nullptr;
}

void HashTrieIterator::up() {
    if(!cursor->parent)
        return;

    if (tuples) {
        tuples = nullptr;
    } else {
        entry = prev_entries.top();
        prev_entries.pop();
        cursor = cursor->parent;
    }
}

void HashTrieIterator::down() {
    if(entry->points_to_tuple_list) {
        tuples = entry->tuple_list_ptr;
        return;
    } else {
        prev_entries.push(entry);
        cursor = entry->hash_trie_node_ptr;
        entry = cursor->head->next;
    }
}

bool HashTrieIterator::next() {
    if(!entry->next) {
        return false;
    } else {
        entry = entry->next;
        return true;
    }
}
bool HashTrieIterator::lookup(int hash) {
    // Local variables
    int index = hash >> cursor->shift;
    int allocated_size = cursor->allocated_size;
    HashTrieEntry* cursor_entry = cursor->hash_table[index];

    // if there is no entry at that index, item is definitely not in hash table
    if(!cursor_entry) {
        std::cout << "no entry at index " << index << std::endl;
        return false;
    }

    // Found matching hash
    if(cursor_entry->hash == hash) {
        entry = cursor_entry;
        return true;
    }
    
    int i = (index+1);
    
    for(; i<allocated_size; ++i) {
        cursor_entry = cursor->hash_table[i];
        if(!cursor_entry){
            return false;
        }
        if(cursor_entry->hash == hash) {
            entry = cursor_entry;
            return true;
        }
    }

    // if there is an entry at given hash table index with different hash, we have a collision in hashes,
    // therefore we need to iterate over the next entries until we find a matching hash or an empty entry
    for(i=0; i<=index; ++i) {
        cursor_entry = cursor->hash_table[i];
        if(!cursor_entry){
            return false;
        }
        if(cursor_entry->hash == hash) {
            entry = cursor_entry;
            return true;
        }
    }
    return false;

}

std::ostream &operator<<(std::ostream &os, const HashTrieIterator &it) {
    os << *(it.entry);
    return os;
}