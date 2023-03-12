#include "table.h"
#include <fstream>
#include <iostream>
using namespace std;

Table::Table(const std::string &tsv_filename, int num_cols) : num_columns(num_cols) {
    ifstream infile(tsv_filename);
    if (!infile.is_open()) {
        cerr<<("the file " + tsv_filename + " cannot be opened.");
        exit(-1);
    }
    bool reach_end = false;
    while (!reach_end) {
        vector<int> new_row(num_cols);
        for (int i = 0; i < num_cols; i++) {
            if (!(infile >> new_row[i])) {
                reach_end = true;
                break;
            }
        }
        if (!reach_end) {
            data.push_back(new_row);
        }
    }
    infile.close();
}

void Table::append_row(const std::vector<int> &new_row) {
    if (new_row.size() != num_columns) {
        cerr<<("The new row is of a different size.");
        exit(-1);
    }

    data.push_back(new_row);
}

ostream &operator<<(ostream &os, const Table &table) {
    for (int row_id = 0; row_id < table.data.size(); row_id++) {
        const vector<int> &row = table.data[row_id];
        for (int col_id = 0; col_id < table.num_columns; col_id++) {
            os << row[col_id];
            if (col_id != table.num_columns - 1) {
                os << ' ';
            }
        }
        if (row_id != table.data.size() - 1) {
            os << endl;
        }
    }
    return os;
}
