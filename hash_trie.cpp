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
    parent_hash = 0;
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

HashTrieEntry* HashTrieNode::build(int idx, int *indeces, int len, TupleList *L) {
    if(idx >= len) {
        HashTrieEntry *leaf = new HashTrieEntry();
        leaf->points_to_tuple_list = true;
        leaf->tuple_list_ptr = L;
        return leaf;
    }

    int p = ceil(log2(1.25f*L->length()));
    long size = pow(2, p);
    HashTrieNode *M = new HashTrieNode(size, p);
    
    while(!L->empty()) {
        Tuple *t = L->pop_left();
        M->insert_tuple_at(M->calc_hash(t->data[indeces[idx]]), t);
    }

    ++idx;
    for(int i = 0; i < M->allocated_size; ++i) {
        if(!M->hash_table[i].tuple_list_ptr)
            continue;

        if(M->hash_table[i].tuple_list_ptr->empty())
            continue;
        
        TupleList *L_next = new TupleList(M->hash_table[i].tuple_list_ptr);
        HashTrieEntry *M_next = build(idx, indeces, len, L_next);

        if(M_next->points_to_tuple_list) {
            M->hash_table[i].points_to_tuple_list = true;
            M->hash_table[i].hash_trie_node_ptr = nullptr;
            M->hash_table[i].tuple_list_ptr = M_next->tuple_list_ptr;
        } else {
            M->hash_table[i].tuple_list_ptr = nullptr;
            M->hash_table[i].points_to_tuple_list = false;
            M->hash_table[i].hash_trie_node_ptr = M_next->hash_trie_node_ptr;
            M->hash_table[i].hash_trie_node_ptr->parent = M;
            M->hash_table[i].hash_trie_node_ptr->parent_hash = i;
        }

        free(M_next);
    }

    HashTrieEntry *entry = new HashTrieEntry();
    entry->points_to_tuple_list = false;
    entry->hash_trie_node_ptr = M;
    return entry;
}

HashTrieNode* HashTrieNode::build(Table *table, std::vector<std::string> attributes) {
    std::vector<int> indeces;

    for(std::string s1 : attributes) {
        int i = 0;
        for(std::string s2 : table->get_attributes()) {
            if(!s1.compare(s2)) {
                indeces.push_back(i);
                break;
            }
            ++i;
        }
    }

    TupleList *L = new TupleList(*table);
    HashTrieEntry *head = build(0, indeces.data(), indeces.size(), L);
    HashTrieNode *node = head->hash_trie_node_ptr;
    free(head);
    return node;
}

HashTrieIterator::HashTrieIterator(HashTrieNode *node, std::string name) : name(name) {
    bucket = 0;
    hash = 0;
    size = node->size();
    cursor = node;
    tuples = nullptr;

    while(bucket < cursor->allocated_size) {
        if(cursor->hash_table[bucket].hash_trie_node_ptr || cursor->hash_table[bucket].tuple_list_ptr)
            break;
        ++hash;
    }

    if(cursor->hash_table[bucket].points_to_tuple_list)
        tuples = cursor->hash_table[bucket].tuple_list_ptr;

    hash = cursor->hash_table[bucket].hash;
}

void HashTrieIterator::up() {
    if(!cursor->parent)
        return;

    if(tuples) {
        tuples = nullptr;
    } else {
        bucket = previous_bucket;
        cursor = cursor->parent;
        hash = cursor->hash_table[bucket].hash;
        size = cursor->size();
    }
}

void HashTrieIterator::down() {
    if(cursor->hash_table[bucket].points_to_tuple_list) {
        tuples = cursor->hash_table[bucket].tuple_list_ptr;
        size = tuples->length();
    } else if(cursor->hash_table[bucket].hash_trie_node_ptr){
        previous_bucket = bucket;
        cursor = cursor->hash_table[bucket].hash_trie_node_ptr;
        size = cursor->size();
        bucket = 0;
        
        while(bucket < cursor->allocated_size) {
            if(cursor->hash_table[bucket].hash_trie_node_ptr)
                break;
            ++bucket;
        }

        hash = cursor->hash_table[bucket].hash;
    }
}

bool HashTrieIterator::next() {
    int old = bucket;
    ++bucket;
    while(bucket < size) {
        if(cursor->hash_table[bucket].hash_trie_node_ptr) {
            if(cursor->hash_table[bucket].points_to_tuple_list) {
                tuples = cursor->hash_table[bucket].tuple_list_ptr;
            }

            hash = cursor->hash_table[bucket].hash;
            return true;
        }

        ++bucket;
    }

    bucket = old;
    hash = cursor->hash_table[bucket].hash;
    return false;
}

bool HashTrieIterator::lookup(uint64_t hash) {
    //std::cout << " lu[" << bucket%cursor->allocated_size%modulus << "]: " << cursor->hash_table[bucket%cursor->allocated_size%modulus].hash_trie_node_ptr << " " << cursor->hash_table[bucket%cursor->allocated_size].tuple_list_ptr;

    int index = hash >> cursor->shift;

    // if there is no entry at that index, item is definitely not in hash table
    if(!cursor->hash_table[index].hash_trie_node_ptr) {
        std::cout << "no entry at index " << index << std::endl;
        return false;
    }

    // if there is an entry at given hash table index with different hash, we have a collision in hashes,
    // therefore we need to iterate over the next entries until we find a matching hash or an empty entry
    if(cursor->hash_table[index].hash != hash) {
        int start = index;

        do {
            index = (index+1)%cursor->allocated_size;
            if(cursor->hash_table[index].hash == hash || !cursor->hash_table[index].hash_trie_node_ptr) {
                bucket = index;
                hash = cursor->hash_table[bucket].hash;
                return true;
            }
        } while(start != index && cursor->hash_table[index].hash != hash);

        return false;
    }

    // else everything worked, there was a matching entry at index
    bucket = index;
    hash = cursor->hash_table[bucket].hash;

    return true;
}

std::ostream &operator<<(std::ostream &os, const HashTrieIterator &it) {
    os << "bucket: " << it.bucket << endl;

    return it.cursor->hash_table[it.bucket].print_with_indent(os, 1);
}