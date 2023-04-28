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

void HashTrieNode::insert_tuple_at(unsigned long index, TupleListNode *node) {
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
    return node.print_with_indent(os, 0) << endl << "end\n";
}


std::ostream & HashTrieNode::print_with_indent(std::ostream &os, int num_tabs) const {
    for (int i = 0; i < allocated_size; i++) {
        os << "index " << i << ": " << std::endl;
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

    long size = pow(2, ceil(1.25f*L->length()));
    HashTrieNode *M = new HashTrieNode(4);
    
    while(!L->empty()) {
        TupleListNode *t = L->pop_left();
        long B = M->calc_hash(t->data[indeces[idx]]);
        M->insert_tuple_at(B, t);
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
            M->hash_table[i].hash_trie_node_ptr->hash = i;
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

    std::cout << std::endl;

    TupleList *L = new TupleList(*table);
    HashTrieEntry *head = build(0, indeces.data(), indeces.size(), L);
    HashTrieNode *node = head->hash_trie_node_ptr;
    free(head);
    return node;
}

HashTrieIterator::HashTrieIterator(HashTrieNode *node) {
    this->hash = 0;
    size = node->allocated_size;
    cursor = node;
    tuples = nullptr;

    while(hash < size) {
        if(cursor->hash_table[hash].hash_trie_node_ptr || cursor->hash_table[hash].tuple_list_ptr)
            break;
        ++hash;
    }

    if(cursor->hash_table[hash].points_to_tuple_list)
        tuples = cursor->hash_table[hash].tuple_list_ptr;
}

void HashTrieIterator::up() {
    hash = cursor->hash;
    cursor = cursor->parent;
    size = cursor->allocated_size;
}

void HashTrieIterator::down() {
    if(cursor->hash_table[hash].points_to_tuple_list) {
        tuples = cursor->hash_table[hash].tuple_list_ptr;
    } else if(cursor->hash_table[hash].hash_trie_node_ptr){
        cursor = cursor->hash_table[hash].hash_trie_node_ptr;
        size = cursor->allocated_size;
        hash = 0;

        if(cursor->hash_table[hash].points_to_tuple_list) {
            tuples = cursor->hash_table[hash].tuple_list_ptr;
        }
    }
}

bool HashTrieIterator::next() {
    ++hash;
    while(hash < size) {
        if(cursor->hash_table[hash].hash_trie_node_ptr || cursor->hash_table[hash].tuple_list_ptr) {
            if(cursor->hash_table[hash].points_to_tuple_list) {
                tuples = cursor->hash_table[hash].tuple_list_ptr;
                return true;
            }
        }
            
        ++hash;
    }

    --hash;
        
    return false;
}

bool HashTrieIterator::lookup(long bucket) {
    if(hash < cursor->allocated_size) {
        if(cursor->hash_table[hash].points_to_tuple_list && cursor->hash_table[hash].tuple_list_ptr == nullptr) {
            return false;
        }

        hash = cursor->hash;
    } else
        return false;

    return true;
}

std::ostream &operator<<(std::ostream &os, const HashTrieIterator &it) {
    os << "bucket: " << it.hash << endl;

    return it.cursor->hash_table[it.hash].print_with_indent(os, 1);
}