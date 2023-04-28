#include <iostream>
#include "table.h"
#include "tuple_list.h"
#include "hash_trie.h"
#include "query.h"
using namespace std;

int main(int argc, char **argv) {
    // Load the data (same as example in paper)
    Table *R1 = new Table("./data/r1.tsv");
    Table *R2 = new Table("./data/r2.tsv");
    Table *R3 = new Table("./data/r3.tsv");
    
    // The tables and attributes that we want to participate in the join query
    Table *tables[] = {R1, R2, R3};
    std::vector<std::string> attributes = {"v1", "v2", "v3"};
    
    // Executes algorithm 2 for each table
    JoinQuery query(tables, 3, attributes);

    // Executes algorithm 3 and retrieves join query results
    TupleList *result = query.exec();

    // Print results
    std::cout << *result << std::endl;;
}
