#include "query.h"

JoinQuery::JoinQuery(const Table **tables, int num_tables, const std::vector<std::string>& join_attributes) : num_tables(num_tables) {
    this->tables = tables;
    results = nullptr;
    hash_tries = (HashTrieNode**)malloc(num_tables*sizeof(HashTrieNode*));
    iterators = (HashTrieIterator**)malloc(num_tables*sizeof(HashTrieIterator*));
    // TODO: optimize this copy statement
    this->attributes = join_attributes;
    int total_attributes = 0;

    num_attributes = join_attributes.size();

    std::vector<std::string> all_attributes = std::vector<std::string>();

    for(int i = 0; i < num_tables; ++i) {
        hash_tries[i] = HashTrieNode::build(this->tables[i], attributes);
        iterators[i] = new HashTrieIterator(hash_tries[i], this->tables[i]->name);

        for(const std::string& a : tables[i]->get_attributes())
            all_attributes.push_back(a);

        total_attributes += tables[i]->get_num_attributes();

        #ifdef DEBUG
        std::cout << tables[i]->name << std::endl;
        std::cout << *hash_tries[i] << std::endl;
        #endif
    }
}

JoinQuery::~JoinQuery() {
    for(int i = 0; i < num_tables; ++i) {
        delete iterators[i];
        delete hash_tries[i];
    }

    free(hash_tries);
    free(iterators);
}

Table *JoinQuery::exec() {
    if(results != nullptr) {
        delete results;
        results = nullptr;
    }

    enumerate(0);

    return results;
}

void JoinQuery::enumerate(int index) {
    if(index < num_attributes) {
        #ifdef DEBUG
        std::cout << "[" << attributes[index] << "]: ";
        for(int i = 0; i < num_tables; ++i) {
            std::cout << tables[i]->name << " " << iterators[i]->get_hash() << ", ";
        }
        std::cout << std::endl;
        std::cout << "[" << attributes[index] << "] joining ";
        #endif

        std::vector<HashTrieIterator*> join;
        for(int i = 0; i < num_tables; ++i) {
            if(tables[i]->contains_attribute(attributes[index])) {
                join.push_back(iterators[i]);

                #ifdef DEBUG
                std::cout << tables[i]->name << " ";
                #endif
            }
        }

        int min = 0x7fffffff;
        int i_scan = 0;

        if(join.size() > 1) {
            for(int i = 0; i < join.size(); ++i) {
                if(join[i]->get_size() < min) {
                    min = join[i]->get_size();
                    i_scan = i;
                }
            }
        }

        #ifdef DEBUG
        std::cout << std::endl;
        #endif

        do {
            #ifdef DEBUG
            std::cout << "[" << attributes[index] << "] lead " << join[i_scan]->name << std::endl;
            #endif

            for(int j = 0; j < join.size(); ++j) {
                if(j == i_scan)
                    continue;

                #ifdef DEBUG
                std::cout << "[" << attributes[index] << "] looking for " <<  join[i_scan]->get_hash() << " in " << join[j]->name;
                #endif

                if(!join[j]->lookup(join[i_scan]->get_hash())) {
                    #ifdef DEBUG
                    std::cout << " - not found\n";
                    std::cout << *join[j] << std::endl;
                    #endif

                    goto next;
                }

                #ifdef DEBUG
                std::cout << " - found " << join[j]->get_hash() << "\n";
                #endif
            }

            for(int j = 0; j < join.size(); ++j) {
                # ifdef DEBUG
                std::cout<< "descend "<<join[j]->name<<" from "<<join[j]->get_hash();
                #endif
                join[j]->down();
                # ifdef DEBUG
                std::cout<< " to "<<join[j]->get_hash()<<std::endl;
                #endif
            }

            enumerate(index+1);

            for(int j = 0; j < join.size(); ++j) {
                # ifdef DEBUG
                std::cout<< "ascend "<<join[j]->name<<" from "<<join[j]->get_hash();
                #endif
                join[j]->up();
                # ifdef DEBUG
                std::cout<< " to "<<join[j]->get_hash()<<std::endl;
                #endif
            }
            
            next:
            int j = 0;
        } while (join[i_scan]->next());

    } else {
        #ifdef DEBUG
        std::cout << "[*] finished: ";
        for(int i = 0; i < num_tables; ++i) {
            std::cout << tables[i]->name << " " << iterators[i]->get_hash() << ", ";
        }
        std::cout << std::endl;
        #endif

        JoinedTupleBuilder builder(tables, num_tables, attributes);

        for(int i = 0; i < num_tables; ++i) { 
            if(iterators[i]->cursor->hash_table[iterators[i]->get_hash()].points_to_tuple_list) {
                if(iterators[i]->get_tuples()) {
                    TupleList *list = new TupleList(iterators[i]->get_tuples());
                    
                    while(!list->empty())
                        builder.add_tuple(i, list->pop_left());
                }
            }
        }

        std::vector<std::vector<int>> r = builder.build();

        if(!results)
            results = new Table("Join Result", builder.get_attributes());

        for(const std::vector<int>& row : r) {
            # ifdef DEBUG
            std::cout << "[*] appending row: ";
            for(int i = 0; i < row.size(); ++i) {
                std::cout << row[i] << ", ";
            }
            std::cout<< std::endl;
            # endif
            results->append_row(row);
        }
    }
}

JoinedTupleBuilder::JoinedTupleBuilder(const Table **tables, int num_tables, const std::vector<std::string> & join_attributes) : num_tables(num_tables), tables(tables), join_attributes(join_attributes) {
    occupied = (bool*)calloc(num_tables, sizeof(bool));
    std::vector<bool> attribute_taken = std::vector<bool>(join_attributes.size(), false);
    start_idx = (int*)malloc(sizeof(int)*num_tables);
    start_idx[0] = 0;

    total_attributes = 0;

    for(int i = 0; i < num_tables; ++i) {
        int num_attr = 0;
        std::vector<bool> pick;
        for(int j = 0; j < tables[i]->get_num_attributes(); ++j) {
            bool include = true;
            for(int k = 0; k < join_attributes.size(); ++k) {
                if(!tables[i]->get_attributes()[j].compare(join_attributes[k])) {
                    if(attribute_taken[k]) {
                        include = false;
                        break;
                    } 
                    attribute_taken[k] = true;
                }
            }

            if(include) {
                pick.push_back(true);
                attributes.push_back(tables[i]->get_attributes()[j]);
                num_attr++;
            } else {
                pick.push_back(false);
            }
        }

        pick_attr.push_back(pick); 

        total_attributes += num_attr;

        if(i < num_tables - 1)
            start_idx[i+1] = start_idx[i] + num_attr;
    }
}

void JoinedTupleBuilder::add_tuple(int table_idx, Tuple *tuple) {
    if(occupied[table_idx]) {
        int len = data.size();
        for(int i = 0; i < len; ++i) {
            data.push_back(data[i]);
        }
    } else {
        occupied[table_idx] = true;
    }

    if(data.size() == 0) {
        std::vector<int> r(total_attributes, 0);
        data.push_back(r);
    }

    for(int i = 0; i < data.size(); ++i) {
        int tuple_len = tuple->tuple_size;
        int idx = 0;
        for(int j = 0; j < tuple_len; ++j) {
            if(pick_attr[table_idx][j]) {
                data[i][idx + start_idx[table_idx]] = tuple->data[j];
                idx++;
            }
        }
    }
}

std::vector<std::vector<int>> JoinedTupleBuilder::build() {
    return data;
}