#ifndef TEAM02_JOINED_TABLE_BUILDER_H
#define TEAM02_JOINED_TABLE_BUILDER_H
#include <vector>
#include "hash_trie.h"
#include "col_immutable_table.h"

struct JoinedTableBuilder {
    std::vector<int>* columns;


    // for example, suppose we have 3 tables with 2, 3, and 4 columns respectively
    // then table_start_indices = [0, 2, 5], num_tables = 3, num_columns = 9
    const int num_tables;
    int num_columns;
    int* table_start_indices;

    // disable copy constructor and copy assignment operator
    JoinedTableBuilder(const JoinedTableBuilder &) = delete;
    JoinedTableBuilder &operator=(const JoinedTableBuilder &) = delete;

    JoinedTableBuilder(HashTrieIterator *const *iterators, int num_tables);
    ~JoinedTableBuilder();

    void append_rows(HashTrieIterator *const* hash_trie_iterators);
    // should be called off the critical path as it is O(num_columns * num_rows)
    void clear_columns();

    ColImmutableTable* compact(const Table *const *tables, int num_join_attributes);
private:
    void iterate_from(int index, Tuple** tuple_iterators, HashTrieIterator *const *hash_trie_iterators);
};


#endif //TEAM02_JOINED_TABLE_BUILDER_H
