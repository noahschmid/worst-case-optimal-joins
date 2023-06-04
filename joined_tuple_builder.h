#ifndef TEAM02_JOINED_TUPLE_BUILDER_H
#define TEAM02_JOINED_TUPLE_BUILDER_H

#include "table.h"
#include "tuple_list.h"
#include <vector>
#include <string>


class JoinedTupleBuilder {
public:
    JoinedTupleBuilder(const Table **tables, int num_tables, const std::vector<std::string> & join_attributes);

    ~JoinedTupleBuilder() {
        free(start_idx_h);
        free(start_idx_v);
    }

    void duplicate(int n);
    void add_tuple(int table_idx, const Tuple *tuple);
    std::vector<std::vector<int>> build();

    std::vector<std::string> get_attributes() { return attributes; }

private:
    const Table **tables;
    int *start_idx_v; // vertical start index
    int *start_idx_h; // horizontal start index
    std::vector<std::vector<bool>> pick_attr;
    std::vector<std::vector<int>> data;
    std::vector<std::string> join_attributes;
    std::vector<std::string> attributes;
    int num_tables;
    int total_attributes;
};


#endif //TEAM02_JOINED_TUPLE_BUILDER_H
