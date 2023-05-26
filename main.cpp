#include <iostream>
#include "table.h"
#include "database.h"

using namespace std;

int main(int argc, char **argv) {
    // Initialize database
    Database *db = new Database();
    db->load_table("./data/mini_mini_data_1.tsv", "R1");
    db->load_table("./data/mini_mini_data_2.tsv", "R2");
    db->load_table("./data/mini_mini_data_3.tsv", "R3");

    // Executes algorithms 2 & 3 and retrieves join query results
    Table *result = db->query("JOIN R1,R2,R3 ON v1,v2,v3");

    // Print results
    if(result)
        std::cout << *result << std::endl;

    delete result;
    delete db;
}
