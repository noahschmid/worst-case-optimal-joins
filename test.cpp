#include <iostream>
#include "table.h"
#include "tuple_list.h"
#include "hash_trie.h"
#include "query.h"
#include "database.h"

using namespace std;

int main(int argc, char **argv) {
    // Load the data (same as example in paper)
    Table *R1 = new Table("./data/r1.tsv", "R1");
    Table *R2 = new Table("./data/r2.tsv", "R2");
    Table *R3 = new Table("./data/r3.tsv", "R3");
    Table *expected_result = new Table("./data/expected.tsv", "Join Result");

    std::vector<std::string> join_attributes = {"v1", "v2", "v3"};
    Table *tables[] = { R1, R2, R3 };


    // Executes algorithms 2 & 3 and retrieves join query results
    JoinQuery *query = new JoinQuery(tables, 3, join_attributes);
    Table *result = query->exec();

    if(result->equals(expected_result)) {
        std::cout << "Result correct.\n";
    } else {
        std::cout << "Wrong result.\n";
    }

    delete query;
    delete result;
    delete R1;
    delete R2;
    delete R3;
    delete expected_result;
}
