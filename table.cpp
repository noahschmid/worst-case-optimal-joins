#include "table.h"
#include <fstream>

using namespace std;

Table::Table(const std::string &tsv_filename, int num_cols) : num_columns(num_cols) {
    ifstream infile(tsv_filename);
    if (!infile.is_open()) {
        throw std::runtime_error("the file " + tsv_filename + " cannot be opened.");
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
        throw std::invalid_argument("The new row is of a different size.");
    }

    data.push_back(new_row);
}

ostream &operator<<(ostream &os, const Table &table) {
    for (const auto &row: table.data) {
        for (int col_id = 0; col_id < table.num_columns; col_id++) {
            os << row[col_id];
            if (col_id != table.num_columns - 1) {
                os << ' ';
            }
        }
        os << endl;
    }
    return os;
}
