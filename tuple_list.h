#ifndef TEAM02_TUPLE_LIST_H
#define TEAM02_TUPLE_LIST_H

#include <vector>
#include <iostream>
#include "table.h"

// A Tuple is a node holding a tuple, or in other words holding a row in a relational table,
// it also has a pointer to the next Tuple, so that all of them chained together form a linked list.
struct Tuple {
    const int tuple_size;
    int *data;
    Tuple *next;

    Tuple(Tuple *node);

    Tuple(int tuple_size_arg, Tuple *nextNode = nullptr);

    Tuple(const std::vector<int> &row, Tuple *nextNode = nullptr);

    ~Tuple();

    // for debugging purposes
    friend std::ostream &operator<<(std::ostream &os, const Tuple &node);

    // disable copy constructor and copy assignment operator
    Tuple(const Tuple &) = delete;

    Tuple &operator=(const Tuple &) = delete;
};

// Head itself does not store data but points to the first element.
// If head->next == nullptr, then this is an empty list.
// The next node to be inserted should always be at tail->next.
// WARNING: all nodes in the tuple list should be dynamically created, i.e. via new.
struct TupleList {
    Tuple *head;
    Tuple *tail;

    TupleList();

    // turn a table into a TupleList
    TupleList(const Table &table);

    void append(Tuple *node);

    void merge(TupleList *list);

    Tuple *pop_left();

    ~TupleList();

    bool empty() const { return head->next == nullptr; }

    int length();

    // for debugging purposes
    friend std::ostream &operator<<(std::ostream &os, const TupleList &list);
};

#endif //TEAM02_TUPLE_LIST_H
