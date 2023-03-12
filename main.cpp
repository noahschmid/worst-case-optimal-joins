#include <iostream>
#include "table.h"
#include "tuple_list.h"
#include "hash_trie.h"

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
    c = new TupleListNode(vector<int>{9, 10, 12, 12});


    HashTrieNode *hash_trie_node = new HashTrieNode(10);

    hash_trie_node->insert_tuple_at(a, hash_trie_node->hash(1));
    hash_trie_node->insert_tuple_at(b, hash_trie_node->hash(5));
    hash_trie_node->insert_tuple_at(c, hash_trie_node->hash(1));

    HashTrieNode root(5);

    root.insert_hash_trie_node_at(hash_trie_node, root.hash(2));

    cout << root << endl;
}
