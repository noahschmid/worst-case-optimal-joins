#include "query.h"
#include "col_immutable_table.h"
//#define DEBUG 1

JoinQuery::JoinQuery(const Table **tables, int num_tables, const std::vector<std::string>& join_attributes) : num_tables(num_tables) {
    this->tables = tables;
    results = nullptr;
    hash_tries = (HashTrieNode**)malloc(num_tables*sizeof(HashTrieNode*));
    iterators = (HashTrieIterator**)malloc(num_tables*sizeof(HashTrieIterator*));
    // TODO: optimize this copy statement
    this->attributes = join_attributes;

    num_attributes = join_attributes.size();

    for(int i = 0; i < num_tables; ++i) {
        hash_tries[i] = HashTrieNode::build(this->tables[i], this->attributes);
        iterators[i] = new HashTrieIterator(hash_tries[i], this->tables[i]->name);

        #ifdef DEBUG
        std::cout << tables[i]->name << std::endl;
        std::cout << *hash_tries[i] << std::endl;
        #endif
    }
}

JoinQuery::~JoinQuery() {
    for(int i = 0; i < num_tables; ++i) {
        delete iterators[i];
        iterators[i] = nullptr;
        delete hash_tries[i];
        hash_tries[i] = nullptr;
    }

    free(hash_tries);
    hash_tries = nullptr;
    free(iterators);
    iterators = nullptr;

    if(joined_table_builder != nullptr) {
        delete joined_table_builder;
        joined_table_builder = nullptr;
    }
}

ColImmutableTable *JoinQuery::exec() {
    enumerate(0);
    results = joined_table_builder->compact(tables, num_attributes);
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

        int min = 0x7fffffff;
        int i_scan = 0;

        std::vector<HashTrieIterator*> join;
        for(int i = 0; i < num_tables; ++i) {
            if(tables[i]->contains_attribute(attributes[index])) {
                HashTrieIterator*it = iterators[i];
                join.push_back(it);
                const unsigned long size = it->get_size();
                if(size < min) {
                    min = size;
                    i_scan = join.size()-1;
                }
                #ifdef DEBUG
                std::cout << tables[i]->name << " ";
                #endif
            }
        }


        #ifdef DEBUG
        std::cout << std::endl;
        #endif

        if(join.size() == 0) {
            #ifdef DEBUG
            std::cout << "[" << attributes[index] << "]: no tables to join" << std::endl;
            #endif
            return;
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

            for (int j = 0; j < join.size(); ++j) {
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
        if (joined_table_builder == nullptr) {
            joined_table_builder = new JoinedTableBuilder(iterators, num_tables);
        }
        joined_table_builder->append_rows(iterators);
    }
}

void JoinQuery::clear() {
    if(results != nullptr) {
        delete results;
        results = nullptr;
    }

    // these clear-up work can be done off the critical path
    // side effect: enumerate() moves iterators to the end
    // we reset iterators to have a clean state for next execution
    for(int i = 0; i < num_tables; ++i) {
        iterators[i]->entry = iterators[i]->cursor->head->next;
    }
    delete joined_table_builder;
    joined_table_builder = nullptr;
}