#include <iostream>
#include "table.h"
#include "database.h"

using namespace std;

int main(int argc, char **argv) {
    // Initialize database
    Database db = Database();
    db.load_table("./data/mini_mini_data_1.tsv", "R1");
    db.load_table("./data/mini_mini_data_2.tsv", "R2");
    db.load_table("./data/mini_mini_data_3.tsv", "R3");

    // Executes algorithms 2 & 3 and retrieves join query results
    auto result = db.query("JOIN R1,R2,R3 ON v1,v2,v3");

    // Print results
<<<<<<< HEAD
    if(result) {
        std::cout << *result << std::endl;
    }
=======
    //if(result)
        //std::cout << *result << std::endl;
>>>>>>> baseline

    delete result;
}
