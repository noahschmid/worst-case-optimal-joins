#ifndef TEAM02_TABLE_H
#define TEAM02_TABLE_H

#include <string>
#include <vector>
#include <iostream>

class Table {
public:
    std::string name;

    std::vector<std::vector<int>> data; 

    Table(const std::string &tsv_filename, std::string name);

    Table(std::string name, std::vector<std::string> attributes) : name(name), attributes(attributes), num_attributes(attributes.size()) {}

    void append_row(const std::vector<int> &new_row);

    int get_num_attributes() const { return num_attributes; }

    std::string get_attribute_name(int i) { return attributes[i]; }

    int get_attribute_idx(const std::string& name) const;

    std::vector<std::string> get_attributes() const { return attributes; };

    bool contains_attribute(const std::string& name) const;

    int get_num_rows() const { return data.size(); }

    Table *select(const std::vector<std::string>& attributes) const;
    bool equals(Table *other);

    // for debugging purposes
    friend std::ostream &operator<<(std::ostream &os, const Table &table);

private:
    int num_attributes;
    std::vector<std::string> attributes;
};


#endif //TEAM02_TABLE_H
