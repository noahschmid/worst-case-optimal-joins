#ifndef TEAM02_COL_IMMUTABLE_TABLE_H
#define TEAM02_COL_IMMUTABLE_TABLE_H
#include "table.h"
#include <iostream>

// a column-oriented immutable table: i.e. once constructed, the table shape cannot be modified
class ColImmutableTable {
public:
    const int num_rows;
    const int num_columns;
    std::string * attributes;
    int **data;

    ColImmutableTable(int num_rows, int num_columns);
    ~ColImmutableTable();

    // forbid copy constructor and copy assignment operator
    ColImmutableTable(const ColImmutableTable &) = delete;
    ColImmutableTable &operator=(const ColImmutableTable &) = delete;

    bool operator==(const Table &other) const;
    // for debugging purposes
    friend std::ostream &operator<<(std::ostream &os, const ColImmutableTable &table);

};


#endif //TEAM02_COL_IMMUTABLE_TABLE_H
