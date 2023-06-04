#include <iostream>
#include "table.h"
#include "tuple_list.h"
#include "hash_trie.h"
#include "query.h"
#include "database.h"

using namespace std;

int main(int argc, char **argv) {
    // Load the data (same as example in paper)
    // ****************************TEST1*******************************
    Table R1 = Table("./data/r1.tsv", "R1");
    Table R2 = Table("./data/r2.tsv", "R2");
    Table R3 = Table("./data/r3.tsv", "R3");
    Table expected_result = Table("./data/expected.tsv", "Join Result");
    Table expected_result2 = Table("./data/expected_2.tsv", "Join Result");


    std::vector<std::string> join_attributes = {"v1", "v2", "v3"};
    const Table *tables[] = {&R1, &R2, &R3};


    // Executes algorithms 2 & 3 and retrieves join query results
    JoinQuery query = JoinQuery(tables, 3, join_attributes);
    auto result = query.exec();

    std::cout << "Test 1: ";
    if (*result == expected_result) {
        std::cout << "Pass\n";
    } else {
        std::cout << "Fail\n";
    }
    delete result;
    // ****************************TEST2*******************************
    const Table *tables2[] = {&R1, &R2};
    JoinQuery query2 = JoinQuery(tables2, 2, {"v2"});
    result = query2.exec();


    std::cout << "Test 2: ";
    if (*result == expected_result2) {
        std::cout << "Pass\n";
    } else {
        std::cout << "Fail\n";
    }

    delete result;
    // ****************************TEST3*******************************
    Table mini_mini_data_1 = Table("./data/mini_mini_data_1.tsv", "R1");
    Table mini_mini_data_2 = Table("./data/mini_mini_data_2.tsv", "R2");
    Table mini_mini_data_3 = Table("./data/mini_mini_data_3.tsv", "R3");
    Table expected_result3 = Table("./data/mini_mini_expected_result.tsv", "Join Result");
    const Table *tables3[] = {&mini_mini_data_1, &mini_mini_data_2, &mini_mini_data_3};
    JoinQuery query3 = JoinQuery(tables3, 3, join_attributes);
    result = query3.exec();
    std::cout << "Test 3: ";
    if (*result == expected_result3) {
        std::cout << "Pass\n";
    } else {
        std::cout << "Fail\n";
    }
    delete result;
}
