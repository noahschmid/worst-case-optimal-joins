#include "query.h"

JoinQuery::JoinQuery(Table **tables, int num_tables, std::vector<std::string> attributes) : num_tables(num_tables) {
    this->tables = tables;
    
    hash_tries = (HashTrieNode**)malloc(num_tables*sizeof(HashTrieNode*));
    iterators = (HashTrieIterator**)malloc(num_tables*sizeof(HashTrieIterator*));
    this->attributes = attributes;

    for(int i = 0; i < num_tables; ++i) {
        hash_tries[i] = HashTrieNode::build(this->tables[i], attributes);
        iterators[i] = new HashTrieIterator(hash_tries[i]);

        /*std::vector<std::string> attr = this->tables[i]->get_attributes();
        for(std::string s1 : attr) {

            bool found = false;
            for(std::string s2 : attributes) {
                if(!s1.compare(s2)) {
                    found = true;
                    break;
                }
            }

            if(!found)
                attributes.push_back(s1);
        }*/
    }

    num_attributes = attributes.size();
}

TupleList *JoinQuery::exec() {
    results.clear();
    enumerate(0);


    TupleList *list = new TupleList();
    for(TupleListNode *node : results) {
        list->append(node);
    }
    return list;
}

void JoinQuery::enumerate(int index) {

    if(index < num_attributes) {
        std::vector<HashTrieIterator*> join;
        std::vector<HashTrieIterator*> other;

       // std::cout << "current attribute: " << attributes[index] << std::endl;

        for(int i = 0; i < num_tables; ++i) {
            if(tables[i]->contains_attribute(attributes[index]))
                join.push_back(iterators [i]);
            else
                other.push_back(iterators[i]);
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

        //std::cout << *join[i_scan] << std::endl;

        do {
            for(int j = 0; j < join.size(), j != i_scan; ++j) {
                if(!join[j]->lookup(join[i_scan]->get_hash())) {
                    goto next;
                }
            }
          
            for(int j = 0; j < join.size(), j != i_scan; ++j) {
                join[j]->down();
            }

            enumerate(index+1);

            for(int j = 0; j < join.size(), j != i_scan; ++j) {
                join[j]->up();
            }
            
            next:
            continue;
        } while (join[i_scan]->next());

    } else {
        for(int i = 0; i < num_tables; ++i) {
            iterators[i]->down();
            //std::cout << "R" << i+1 << std::endl;
            if(iterators[i]->cursor->hash_table[iterators[i]->get_hash()].points_to_tuple_list) {
                if(iterators[i]->cursor->hash_table[iterators[i]->get_hash()].tuple_list_ptr) {
                    //result->merge(iterators[i]->cursor->hash_table[iterators[i]->get_hash()].tuple_list_ptr);
                    
                    TupleList *list = iterators[i]->cursor->hash_table[iterators[i]->get_hash()].tuple_list_ptr;
                    
                    while(!list->empty())
                        results.push_back(list->pop_left());
                }
                   //std::cout << *(iterators[i]->cursor->hash_table[iterators[i]->get_hash()].tuple_list_ptr) << std::endl;
            }
                
            /*i
            if(iterators[i]->get_tuples())
                std::cout << *(iterators[i]->get_tuples()) << std::endl;*/
        }
    }
}