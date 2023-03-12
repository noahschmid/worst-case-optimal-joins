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

    cout << "size of the hash trie node: " << hash_trie_node->size() << endl;


    HashTrieNode root(5);

    TupleListNode *d, *e;
    d = new TupleListNode(vector<int>{2, -1, -10, 3});
    e = new TupleListNode(vector<int>{-5, 3, 3, 8});
    root.insert_tuple_at(d, root.hash(2));
    root.insert_tuple_at(e, root.hash(-5));
    // make hash_trie_node a child node of the root, placed at hash of 2.
    root.replace_with_hash_trie_node_at(hash_trie_node, root.hash(2));
    // check out how the hash trie looks so far!
    cout << root << endl;

    cout << "size of the hash trie node: " << root.size() << endl;

    cout << "let's enumerate over the initialized entries in root" << endl;
    HashTrieEntry *curr = root.head->next;
    while (curr) {
        cout << (*curr) << endl;
        curr = curr->next;
    }
}
