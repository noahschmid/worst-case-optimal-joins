#include "database.h"
#include <sstream>

void Database::load_table(const std::string & filename, const std::string & name) {
    tables.push_back(Table(filename, name));
}

Table *Database::query(const std::string& query_string) {
    std::stringstream s(query_string);
    std::string tmp;
    getline(s, tmp, ' ');

    if(!tmp.compare("JOIN")) {
        std::string table_names;
        std::string attributes_string;

        std::vector<std::string> attributes;
        std::vector<const Table*> join_tables;

        getline(s, table_names, ' ');
        getline(s, tmp, ' ');
        getline(s, attributes_string, ' ');

        if(tmp.compare("ON"))
            std::cout << "SiQL error: invalid query\n";

        std::stringstream s1(table_names);

        while(getline(s1, tmp, ',')) {
            for(const Table & t: tables) {
                if(t.name.compare(tmp) == 0) {
                    join_tables.push_back(&t);
                }
            }
        }

        if(join_tables.size() == 0) {
            std::cout << "SiQL error: table not found\n";
            return nullptr;
        }
        
        std::stringstream s2(attributes_string);

        while(getline(s2, tmp, ',')) {
            attributes.push_back(tmp);
        }
        JoinQuery query = JoinQuery(join_tables.data(), join_tables.size(), attributes);
        Table *result = query.exec();

        return result;

    } else if(!tmp.compare("SELECT")) {
        std::string attributes_string;
        std::string table_name;
        std::vector<std::string> attributes;

        getline(s, attributes_string, ' ');
        getline(s, tmp, ' ');
        getline(s, table_name, ' ');

        if(tmp.compare("FROM")){
            std::cout << "SiQL error: invalid query\n";
            return nullptr;
        }

        const Table *target = nullptr;
        for(const Table & t : tables) {
            if(!t.name.compare(table_name)) {
                target = &t;
                break;
            }
        }

        if(target == nullptr) {
            std::cout << "SiQL error: table '" << table_name << "' not found\n";
            return nullptr;
        }

        std::stringstream s2(attributes_string);

        if(attributes_string.find(',') == -1)
            attributes.push_back(attributes_string);

        while(getline(s2, tmp, ',')) {
            attributes.push_back(tmp);
        }

        return target->select(attributes);
    }

    return nullptr;
}