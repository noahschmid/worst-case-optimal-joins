#include "joined_table_builder.h"



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

