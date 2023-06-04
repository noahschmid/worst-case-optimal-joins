#ifndef TEAM02_TABLE_H
#define TEAM02_TABLE_H

#include <string>
#include <utility>
#include <vector>
#include <iostream>

class Table {
public:
    std::string name;

    std::vector<std::vector<int>> data; 

    Table(const std::string &tsv_filename, std::string name);

    Table(std::string name, std::vector<std::string> attributes) : name(std::move(name)), attributes(attributes), num_attributes(attributes.size()) {}

    void append_row(const std::vector<int> &new_row);

    int get_num_attributes() const { return num_attributes; }

    int get_attribute_idx(const std::string& name) const;


    bool contains_attribute(const std::string& name) const;

    int get_num_rows() const { return data.size(); }

    Table *select(const std::vector<std::string>& attributes) const;
    bool operator==(const Table &other) const;

    void serialize(const std::string &filename) const;
    // for debugging purposes
    friend std::ostream &operator<<(std::ostream &os, const Table &table);

    int num_attributes;
    std::vector<std::string> attributes;
};


#endif //TEAM02_TABLE_H
