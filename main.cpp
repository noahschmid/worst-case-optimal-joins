#include <iostream>
#include "table.h"
#include "tuple_list.h"
#include "hash_trie.h"

using namespace std;

int main(int argc, char **argv) {
    // create a relational table from a tsv file; we need to specify the number of columns for this table
    Table table1("./data/table1.tsv", 4);
    // print the table
    cout << table1 << endl;
    // append another row to the table
    table1.append_row(vector<int>{4, 4, 4, 4});
    // print it again
    cout << table1 << endl;

    // materialize a linked list of tuples from the table
    TupleList list(table1);
    cout << list << endl;

    // create 3 tuples represented by TupleListNode
    TupleListNode *a, *b, *c;
    a = new TupleListNode(vector<int>{1, 2, 3, 4});
    b = new TupleListNode(vector<int>{5, 6, 7, 8});
    c = new TupleListNode(vector<int>{1, 10, 12, 12});
    // surely we can print them
    cout << *a << endl;

    // create a hash trie node and insert the above tuples based on the hash value of their 1st attribute
    HashTrieNode *hash_trie_node = new HashTrieNode(10);

    hash_trie_node->insert_tuple_at(a, hash_trie_node->hash(1));
    hash_trie_node->insert_tuple_at(b, hash_trie_node->hash(5));
    hash_trie_node->insert_tuple_at(c, hash_trie_node->hash(1));

    HashTrieNode root(5);

    // make hash_trie_node a child node of the root, placed at hash of 2.
    root.insert_hash_trie_node_at(hash_trie_node, root.hash(2));
    // check out how the hash trie looks so far!
    cout << root << endl;
}
