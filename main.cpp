#include <iostream>
#include "table.h"
#include "tuple_list.h"

using namespace std;

int main(int argc, char **argv) {
    Table table1("./data/table1.tsv", 4);
    cout << table1 << endl;
    table1.append_row(vector<int>{4, 4, 4, 4});
    cout << table1 << endl;
    cout << "num col " << table1.get_num_columns() << " num row " << table1.get_num_rows() << endl;

    TupleListNode *a, *b, *c;
    a = new TupleListNode(vector<int>{1, 2, 3, 4});
    b = new TupleListNode(vector<int>{5, 6, 7, 8});
    c = new TupleListNode(vector<int>{9, 10, 12, 12}, b);

    TupleList list(4);
    list.append(a);
    list.append(c);
    cout << "list " << list << endl;
}
