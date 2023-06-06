#include "col_immutable_table.h"
using namespace std;

ColImmutableTable::ColImmutableTable(int num_rows, int num_columns)
        : num_rows(num_rows), num_columns(num_columns), data(new int *[num_columns]), attributes(new string[num_columns]) {
    for (int i = 0; i < num_columns; ++i) {
        data[i] = new alignas(32) int[num_rows];
    }
}

ColImmutableTable::~ColImmutableTable() {
    for (int i = 0; i < num_columns; ++i) {
        delete[] data[i];
    }
    delete[] data;
    data = nullptr;
    delete[] attributes;
    attributes = nullptr;
}

ostream &operator<<(ostream &os, const ColImmutableTable &col_immutable_table) {
    os << "|";
    for (int i = 0; i < col_immutable_table.num_columns - 1; ++i) {
        os <<  "--------";
    }

    os << "-------|" << std::endl;


    os << "| ";


    for (int i = 0; i < col_immutable_table.num_columns - 1; ++i) {
        os <<  "-------+";
    }

    os << "-------|" << std::endl << "| ";

    for (int i = 0; i < col_immutable_table.num_columns; ++i) {
        os << col_immutable_table.attributes[i] << "\t| ";
    }

    os << endl << "|";
    for (int i = 0; i < col_immutable_table.num_columns - 1; ++i) {
        os <<  "-------+";
    }

    os << "-------|" << std::endl << "| ";

    for (int row_id = 0; row_id < col_immutable_table.num_rows; row_id++) {

        for (int col_id = 0; col_id < col_immutable_table.num_columns; col_id++) {
            os << col_immutable_table.data[col_id][row_id];
            os << "\t| ";
        }
        if (row_id != col_immutable_table.num_rows - 1) {
            os << endl;
            os << "| ";
        }
    }

    os << std::endl << "|";
    for (int i = 0; i < col_immutable_table.num_columns - 1; ++i) {
        os <<  "-------+";
    }

    os << "-------|" << std::endl;
    return os;
}

bool ColImmutableTable::operator==(const Table &other) const {
    // when the number of columns or rows are different, the tables are not equal
    if (other.data.size() != num_rows || other.data[0].size() != num_columns) {
#ifdef DEBUG
        if (other.data.size() != num_rows) {
            std::cerr << "The number of rows are different." << std::endl;
        } else {
            std::cerr << "The number of columns are different." << std::endl;
        }
#endif
        return false;
    }

    for(int row_index = 0; row_index < num_rows; ++row_index) {
        for(int col_index = 0; col_index < num_columns; ++col_index) {
            if(other.data[row_index][col_index] != data[col_index][row_index]) {
#ifdef DEBUG
                std::cout<<"The values at row "<<i<<" and column "<<j<<" are different."<<std::endl;
#endif
                return false;
            }
        }
    }
    return true;
}
