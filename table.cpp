#include "table.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>

using namespace std;

Table::Table(const std::string &tsv_filename, std::string name) : name(std::move(name)) {
    ifstream infile(tsv_filename);
    if (!infile.is_open()) {
        cerr<<("the file " + tsv_filename + " cannot be opened.");
        exit(-1);
    }
    bool reach_end = false;

    // first line should give us the attributes
    std::string line;
    std::getline(infile, line);

    std::stringstream ss(line);
    std::string att;
    while(getline(ss, att, ' ')) {
        
        attributes.push_back(att);
    }
    
    num_attributes = attributes.size();

    while (!reach_end) {
        vector<int> new_row(num_attributes);
        for (int i = 0; i < num_attributes; i++) {
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
    if (new_row.size() != num_attributes) {
        cerr<<("The new row is of a different allocated_size.");
        exit(-1);
    }

    data.push_back(new_row);
}

ostream &operator<<(ostream &os, const Table &table) {
    os << "|";
    for (int i = 0; i < table.num_attributes - 1; ++i) {
        os <<  "--------";
    }

    os << "-------|" << std::endl;


    os << "| ";

    int space = (7*table.num_attributes - table.name.length())/2;
    for(int i = 0; i < space; ++i)
        os << " ";

    os << table.name;

    space = 8*table.num_attributes - 2 - space - table.name.length();

    for(int i = 0; i < space; ++i)
        os << " ";
    
    os << "|" << std::endl << "|";

    for (int i = 0; i < table.num_attributes - 1; ++i) {
        os <<  "-------+";
    }

    os << "-------|" << std::endl << "| ";

    for (std::string attr : table.attributes) {
        os << attr << "\t| ";
    }

    os << endl << "|";
    for (int i = 0; i < table.num_attributes - 1; ++i) {
        os <<  "-------+";
    }

    os << "-------|" << std::endl << "| ";

    for (int row_id = 0; row_id < table.data.size(); row_id++) {
        const vector<int> &row = table.data[row_id];
        for (int col_id = 0; col_id < table.num_attributes; col_id++) {
            os << row[col_id];
            os << "\t| ";
        }
        if (row_id != table.data.size() - 1) {
            os << endl;
            os << "| ";
        }
    }

    os << std::endl << "|";
    for (int i = 0; i < table.num_attributes - 1; ++i) {
        os <<  "-------+";
    }

    os << "-------|" << std::endl;
    return os;
}

bool Table::contains_attribute(const std::string& attr) const {
    for(const std::string& a : attributes) {
        if(a.compare(attr) == 0)
            return true;
    }

    return false;
}

int Table::get_attribute_idx(const std::string & name) const {
    for(int i = 0; i < attributes.size(); ++i) {
        if(attributes[i].compare(name) == 0)
            return i;
    }

    return -1;
}

Table *Table::select(const std::vector<std::string>& attributes) const {
    Table *table = new Table(name, attributes);
    std::vector<int> idxs;

    for(const std::string& a : attributes) {
        int idx = get_attribute_idx(a);
        if(idx != -1)
            idxs.push_back(idx);
    }

    for(std::vector<int> row : data) {
        std::vector<int> new_row;
        for(int i : idxs)
            new_row.push_back(row[i]);
        table->append_row(new_row);
    }

    return table;
}

bool Table::operator==(const Table &other) const {
    // when the number of columns or rows are different, the tables are not equal
    if (other.data.size() != data.size() || other.data[0].size() != data[0].size()) {
        #ifdef DEBUG
        if (other.data.size() != data.size()) {
            std::cerr << "The number of rows are different." << std::endl;
        } else {
            std::cerr << "The number of columns are different." << std::endl;
        }
        #endif
        return false;
    }

    for(int i = 0; i < other.data.size(); ++i) {
        for(int j = 0; j < other.data[i].size(); ++i) {
            if(other.data[i][j] != data[i][j]) {
                #ifdef DEBUG
                std::cout<<"The values at row "<<i<<" and column "<<j<<" are different."<<std::endl;
                #endif
                return false;
            }
        }
    }
    return true;
}

void Table::serialize(const string &filename) const {
    ofstream outfile(filename);
    if (!outfile.is_open()) {
        cerr<<("the file " + filename + " cannot be opened.");
        exit(-1);
    }

    for (const string &attr : attributes) {
        outfile << attr << " ";
    }
    outfile << endl;

    for (const vector<int> &row : data) {
        for (int i = 0; i < row.size(); i++) {
            outfile << row[i];
            if (i != row.size() - 1) {
                outfile << " ";
            }
        }
        outfile << endl;
    }
    outfile.close();
}
