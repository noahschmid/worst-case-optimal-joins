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

    TupleListNode(int tuple_size_arg, TupleListNode *nextNode = nullptr);

    TupleListNode(const std::vector<int> &row, TupleListNode *nextNode = nullptr);

    ~TupleListNode();

    // for debugging purposes
    friend std::ostream &operator<<(std::ostream &os, const TupleListNode &node);

    // disable copy constructor and copy assignment operator
    TupleListNode(const TupleListNode &) = delete;

    TupleListNode &operator=(const TupleListNode &) = delete;
};

// Head itself does not store data but points to the first element.
// If head->next == nullptr, then this is an empty list.
// The next node to be inserted should always be at tail->next.
// WARNING: all nodes in the tuple list should be dynamically created, i.e. via new.
struct TupleList {
    TupleListNode *head;
    TupleListNode *tail;

    TupleList(int tuple_size);

    void append(TupleListNode *node);

    TupleListNode* pop_left();

    ~TupleList();

    bool empty() const { return head->next == nullptr; }

    // for debugging purposes
    friend std::ostream &operator<<(std::ostream &os, const TupleList &list);
};

#endif //TEAM02_TUPLE_LIST_H
