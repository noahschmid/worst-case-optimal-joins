#include <iostream>
#include "table.h"
using namespace std;

int main(int argc, char ** argv) {
    Table table1("./data/table1.tsv", 4);
    cout<<table1;
    table1.append_row(vector<int> {4,4,4,4});
    cout<<table1;
    cout<<"num col "<<table1.get_num_columns()<<" num row "<< table1.get_num_rows()<<endl;
}
