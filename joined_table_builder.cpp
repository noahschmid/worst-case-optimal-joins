#include "joined_table_builder.h"

#include <unordered_set>
#include <string>
#include <immintrin.h>
#include <stdalign.h>

JoinedTableBuilder::JoinedTableBuilder(HashTrieIterator *const *iterators, int num_tables): num_tables(num_tables) {
    // calculate the number of columns by summing the lengths of each tuple in each iterator
    num_columns = 0;
    table_start_indices = new int[num_tables];
    for (int i = 0; i < num_tables; ++i) {
        table_start_indices[i] = num_columns;
        num_columns += iterators[i]->get_tuples()->tuple_size();
    }

    // initialize columns
    columns = new std::vector<int>[num_columns];
}

JoinedTableBuilder::~JoinedTableBuilder() {
    delete[] columns;
    columns = nullptr;
    delete[] table_start_indices;
    table_start_indices = nullptr;
}

void JoinedTableBuilder::append_rows(HashTrieIterator *const *hash_trie_iterators) {
    Tuple** tuple_iterators = new Tuple*[num_tables];
    for (int i = 0; i < num_tables; ++i) {
        tuple_iterators[i] = hash_trie_iterators[i]->get_tuples()->head->next;
    }

    iterate_from(0, tuple_iterators, hash_trie_iterators);

    delete[] tuple_iterators;
}

void JoinedTableBuilder::iterate_from(int index, Tuple** tuple_iterators, HashTrieIterator *const *hash_trie_iterators) {
    if (index == num_tables) {
        // we have reached the end of the tuple_iterators array
        // add the current row to the columns array
        for (int i = 0; i < num_tables; ++i) {
            int start = table_start_indices[i];
            int end = start + tuple_iterators[i]->tuple_size;
            for (int j = start; j < end; ++j) {
                columns[j].push_back(tuple_iterators[i]->data[j - start]);
            }
        }
        return;
    }
    Tuple*& tuple_iterator = tuple_iterators[index];
    while (tuple_iterator != nullptr) {
        iterate_from(index + 1, tuple_iterators, hash_trie_iterators);
        tuple_iterator = tuple_iterator->next;
    }

    // we reached the end of this tuple list; reset for next
    tuple_iterator = hash_trie_iterators[index]->get_tuples()->head->next;
}

void JoinedTableBuilder::clear_columns() {
    for (int i = 0; i < num_columns; ++i) {
        columns[i].clear();
    }
}

ColImmutableTable *JoinedTableBuilder::compact(const Table *const *tables, int num_join_attributes) {
    // create a new table
    int num_rows = columns[0].size();
    ColImmutableTable *col_table = new ColImmutableTable(num_rows, num_columns - num_join_attributes);
    std::unordered_set<std::string> seen_attributes;

    int current_col_index = 0;
    for (int table_index = 0; table_index < num_tables; table_index++) {
        const Table* current_table = tables[table_index];
        for (int col_index = 0; col_index < current_table->num_attributes; col_index++) {
            const std::string& attribute = current_table->attributes[col_index];
            if (seen_attributes.find(attribute) == seen_attributes.end()) {
                col_table->attributes[current_col_index] = attribute;
                seen_attributes.insert(attribute);
                // OPTIMIZATION: added Intel intrinsics
                int col_index_in_builder_columns = table_start_indices[table_index] + col_index;
                int row_index = 0;
                for (; row_index < num_rows - 7; row_index+=8) {
                    __m256i t = _mm256_loadu_si256((__m256i*)&(columns[col_index_in_builder_columns][row_index]));
                    _mm256_store_si256((__m256i*)(col_table->data[current_col_index] + row_index), t);
                }

                for (; row_index < num_rows; row_index++) {
                    col_table->data[current_col_index][row_index] = columns[col_index_in_builder_columns][row_index];
                }
                current_col_index++;
            }
        }
    }
    return col_table;
}

