#include <iostream>
#include "table.h"
#include "tuple_list.h"
#include "hash_trie.h"

using namespace std;

int main(int argc, char **argv) {
    Table *R1 = new Table("./data/table1.tsv", 2);
    HashTrieNode *trie = HashTrieNode::build(R1);
    cout << "Hash Trie: \n" << *trie;
}
