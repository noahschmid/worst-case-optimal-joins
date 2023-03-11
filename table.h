#ifndef TEAM02_TABLE_H
#define TEAM02_TABLE_H

#include <string>
#include <vector>
#include <iostream>

class Table {
private:
    int num_columns;
    std::vector<std::vector<int>> data;
public:
    Table(const std::string &tsv_filename, int num_cols);

    Table(int num_cols) : num_columns(num_cols), data() {}

    void append_row(const std::vector<int>& new_row);

    int get_num_columns() const { return num_columns; }

    int get_num_rows() const { return data.size(); }

    int get_data_at(int row, int col) const { return data[row][col]; }

    // for debugging purposes
    friend std::ostream& operator<<(std::ostream& os, const Table& table);
};


#endif //TEAM02_TABLE_H
