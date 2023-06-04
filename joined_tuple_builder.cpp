#include "joined_tuple_builder.h"

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
        for(int j = 0; j < tables[i]->num_attributes; ++j) {
            bool include = true;
            for(int k = 0; k < join_attributes.size(); ++k) {
                if(tables[i]->attributes[j]==join_attributes[k]) {
                    if(attribute_taken[k]) {
                        include = false;
                        break;
                    }

                    attribute_taken[k] = true;
                }
            }

            if(include) {
                pick.push_back(true);
                attributes.push_back(tables[i]->attributes[j]);
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
