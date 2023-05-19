#include "hash_trie.h"

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

void HashTrieNode::insert_tuple_at(uint64_t hash, Tuple *node) {
    int index = hash >> shift;

    // in case of hash collision find next free spot
    while(hash_table[index].hash != hash && hash_table[index].tuple_list_ptr) {
        index++;
        index %= allocated_size;
    }

    // TODO: once we are confident with implementation we can remove this check to speed up
    if (!hash_table[index].points_to_tuple_list) {
        std::cerr << "This entry already points to another hash trie node. Shouldn't append a tuple here.";
        exit(-1);
    }

    hash_table[index].hash = hash;
    HashTrieEntry &entry = hash_table[index];
    
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
    return node.print_with_indent(os, 0) << endl << "end\n";
}


std::ostream & HashTrieNode::print_with_indent(std::ostream &os, int num_tabs) const {
    for (int i = 0; i < allocated_size; i++) {
        //if(!hash_table[i].tuple_list_ptr && !hash_table[i].hash_trie_node_ptr)
        //    continue;
        os << "index " << i << " [" << hash_table[i].hash << "]: " << std::endl;
        for (int j = 0; j < num_tabs + 1; j++) {
            os << "   ";
        }
        hash_table[i].print_with_indent(os, num_tabs+1);
        if (i != allocated_size - 1) {
            os << endl;
            for (int j = 0; j < num_tabs; j++) {
                os << "   ";
            }
        }
    }
    return os;
}

HashTrieEntry* HashTrieNode::build(int idx, const int *indices, int len, TupleList *L) {
    if(idx >= len) {
        HashTrieEntry *leaf = new HashTrieEntry();
        leaf->points_to_tuple_list = true;
        leaf->tuple_list_ptr = L;
        return leaf;
    }

    int p = ceil(log2(1.25f*L->length()));
    unsigned long size = pow(2, p);
    HashTrieNode *M = new HashTrieNode(size, p);
    
    while(!L->empty()) {
        Tuple *t = L->pop_left();
        M->insert_tuple_at(M->calc_hash(t->data[indices[idx]]), t);
    }

    ++idx;
    HashTrieEntry *curr = M->head;
    while (curr->next) {
        curr = curr->next;
        HashTrieEntry *M_next = build(idx, indices, len, curr->tuple_list_ptr);


        if(M_next->points_to_tuple_list) {
            curr->points_to_tuple_list = true;
            curr->hash_trie_node_ptr = nullptr;
            curr->tuple_list_ptr = M_next->tuple_list_ptr;
            M_next->tuple_list_ptr = nullptr;
        } else {
            curr->tuple_list_ptr = nullptr;
            curr->points_to_tuple_list = false;
            curr->hash_trie_node_ptr = M_next->hash_trie_node_ptr;
            curr->hash_trie_node_ptr->parent = M;
            M_next->hash_trie_node_ptr = nullptr;
        }

        delete M_next;
    }

    HashTrieEntry *entry = new HashTrieEntry();
    entry->points_to_tuple_list = false;
    entry->hash_trie_node_ptr = M;
    return entry;
}

HashTrieNode* HashTrieNode::build(const Table *table, const std::vector<std::string>& attributes) {
    std::vector<int> indices;

    for(const std::string& s1 : attributes) {
        int i = 0;
        for(const std::string& s2 : table->get_attributes()) {
            if(!s1.compare(s2)) {
                indices.push_back(i);
                break;
            }
            ++i;
        }
    }

    TupleList *L = new TupleList(*table);
    HashTrieEntry *head = build(0, indices.data(), indices.size(), L);
    // build will move all the tuples from L to the hash trie. So we can delete L now.
    delete L;
    HashTrieNode *node = head->hash_trie_node_ptr;
    free(head); // valgrind warning: mismatched free() 
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

bool HashTrieIterator::lookup(uint64_t hash) {

    int index = hash >> cursor->shift;

    // if there is no entry at that index, item is definitely not in hash table
    if(!cursor->hash_table[index].isInitialized()) {
        std::cout << "no entry at index " << index << std::endl;
        return false;
    }

    // if there is an entry at given hash table index with different hash, we have a collision in hashes,
    // therefore we need to iterate over the next entries until we find a matching hash or an empty entry
    if(cursor->hash_table[index].hash != hash) {
        int start = index;

        do {
            index = (index+1)%cursor->allocated_size;
            if(cursor->hash_table[index].hash == hash && cursor->hash_table[index].isInitialized()) {
                entry = &cursor->hash_table[index];
                return true;
            }
        } while(start != index);

        return false;
    }

    // else everything worked, there was a matching entry at index
    entry = &cursor->hash_table[index];
    return true;
}

std::ostream &operator<<(std::ostream &os, const HashTrieIterator &it) {
    os << *(it.entry);
    return os;
}