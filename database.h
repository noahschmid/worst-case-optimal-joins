#ifndef TEAM02_DB_H
#define TEAM02_DB_H

#include "table.h"
#include "query.h"

class Database {
public:
    Database() {};
    ~Database() {};

    void load_table(const std::string & filename, const std::string & name);
    Table *query(const std::string & query_string);
private:
    std::vector<Table> tables;
};

#endif // TEAM02_DB_H