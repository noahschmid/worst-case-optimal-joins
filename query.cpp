#include "query.h"
//#define DEBUG 1

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
        std::cout << "[" << attributes[index] << "]: joining ";
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

        #ifdef DEBUG
        std::cout << std::endl;
        #endif

        if(join.size() == 0) {
            #ifdef DEBUG
            std::cout << "[" << attributes[index] << "]: no tables to join" << std::endl;
            #endif
            return;
        }

        if(join.size() > 1) {
            for(int i = 0; i < join.size(); ++i) {
                if(join[i]->get_size() < min) {
                    min = join[i]->get_size();
                    i_scan = i;
                }
            }
        }

        do {
            #ifdef DEBUG
            std::cout << "[" << attributes[index] << "]: lead " << join[i_scan]->name << std::endl;
            #endif

            for(int j = 0; j < join.size(); ++j) {
                if(j == i_scan)
                    continue;

                #ifdef DEBUG
                std::cout << "[" << attributes[index] << "]: looking for " <<  join[i_scan]->get_hash() << " in " << join[j]->name;
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
                std::cout<< "[" << attributes[index] << "]: descend "<<join[j]->name<<" from "<<join[j]->get_hash();
                #endif
                join[j]->down();
                # ifdef DEBUG
                std::cout<< " to "<<join[j]->get_hash()<<std::endl;
                #endif
            }

            enumerate(index+1);

            for(int j = 0; j < join.size(); ++j) {
                # ifdef DEBUG
                std::cout<< "[" << attributes[index] << "]: ascend "<<join[j]->name<<" from "<<join[j]->get_hash();
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
                if(iterators[i]->get_tuples()) {
                    builder.duplicate(iterators[i]->get_tuples()->length()-1);

                    const Tuple * curr_tuple = iterators[i]->get_tuples()->head->next;
                    while(curr_tuple) {
                        builder.add_tuple(i, curr_tuple);
                        curr_tuple = curr_tuple->next;
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
    std::vector<bool> attribute_taken = std::vector<bool>(join_attributes.size(), false);
    start_idx_h = (int*)malloc(sizeof(int)*num_tables);
    start_idx_v = (int*)malloc(sizeof(int)*num_tables);
    start_idx_h[0] = 0;

    total_attributes = 0;

    for(int i = 0; i < num_tables; ++i) {
        start_idx_v[i] = 0;
        int num_attr = 0;
        std::vector<bool> pick;
        for(int j = 0; j < tables[i]->get_num_attributes(); ++j) {
            bool include = false;
            for(int k = 0; k < join_attributes.size(); ++k) {
                if(!tables[i]->get_attributes()[j].compare(join_attributes[k])) {
                    if(attribute_taken[k]) {
                        break;
                    } 
                    include = true;
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
            start_idx_h[i+1] = start_idx_h[i] + num_attr;
    }
}

/* duplicate all entries in the table n times */
void JoinedTupleBuilder::duplicate(int n) {
    int len = data.size();

    for(int j = 0; j < n; ++j) {
        for(int i = 0; i < len; ++i) {
            data.push_back(data[i]);
        }
    }
}

void JoinedTupleBuilder::add_tuple(int table_idx, const Tuple *tuple) {
    if(table_idx == 0) {
        std::vector<int> r(total_attributes, 0);
        data.push_back(r);
    }

    int from = start_idx_v[table_idx];
    int to = from;
    if(table_idx == 0) // if it's the first table we feed tuples in strides of one
        to++;
    else {
        from *= start_idx_v[table_idx-1];
        to = from + start_idx_v[table_idx-1];
    }

    for(int i = from; i < to; ++i) {
        int tuple_len = tuple->tuple_size;
        int idx = 0;
        for(int j = 0; j < tuple_len; ++j) {
            if(pick_attr[table_idx][j]) {
                data[i][idx + start_idx_h[table_idx]] = tuple->data[j];
                idx++;
            }
        }
    }

    start_idx_v[table_idx]++;

    #ifdef DEBUG
    std::cout << "..." << std::endl;
    std::cout << *tuple << std::endl;
    for(int i = 0; i < data.size(); ++i) {
        for(int j = 0; j < data[i].size(); ++j) {
            std::cout << data[i][j] << " ";
        }
        std::cout << std::endl;
    }
    #endif

}

std::vector<std::vector<int>> JoinedTupleBuilder::build() {
    return data;
}