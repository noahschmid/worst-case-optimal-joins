#ifndef TEAM02_TUPLE_LIST_H
#define TEAM02_TUPLE_LIST_H

#include <vector>
#include <iostream>
// A TupleListNode is a node holding a tuple, or in other words holding a row in a relational table,
// it also has a pointer to the next TupleListNode, so that all of them chained together form a linked list.
struct TupleListNode {
    const int tuple_size;
    int *data;
    TupleListNode *next;

    TupleListNode(const std::vector<int> &row, TupleListNode *nextNode = nullptr);

    ~TupleListNode();

    // copy constructor and copy assignment operator
    TupleListNode(const TupleListNode &another_node);

    TupleListNode &operator=(const TupleListNode &another_node);

    // move constructor and move assignment operator
    TupleListNode(TupleListNode &&another_node);

    TupleListNode &operator=(TupleListNode &&another_node);

    // for debugging purposes
    friend std::ostream& operator<<(std::ostream& os, const TupleListNode& node);

};

#endif //TEAM02_TUPLE_LIST_H
