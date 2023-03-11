#include "tuple_list_node.h"

TupleListNode::TupleListNode(const std::vector<int> &row, TupleListNode *nextNode) : tuple_size(row.size()),
                                                                                     next(nextNode) {
    data = new int[tuple_size];
    for (int i = 0; i < row.size(); i++) {
        data[i] = row[i];
    }
}

TupleListNode::~TupleListNode() {
    delete[] data;
    data = nullptr;
}

TupleListNode::TupleListNode(const TupleListNode &another_node) : tuple_size(another_node.tuple_size),
                                                                  next(another_node.next) {
    data = new int[tuple_size];
    for (int i = 0; i < tuple_size; i++) {
        data[i] = another_node.data[i];
    }
}

TupleListNode &TupleListNode::operator=(const TupleListNode &another_node) {
    if (tuple_size != another_node.tuple_size) {
        throw std::invalid_argument("assigning a TupleListNode to another with a different size!");
    }

    if (this == &another_node) {
        return *this;
    }
    next = another_node.next;
    // since both data have the same size (tuple_size) then no need to delete old data and reallocate new space
    for (int i = 0; i < tuple_size; i++) {
        data[i] = another_node.data[i];
    }
    return *this;
}

TupleListNode::TupleListNode(TupleListNode &&another_node) : tuple_size(another_node.tuple_size),
                                                             next(another_node.next) {
    data = another_node.data;
    another_node.data = nullptr;
}

TupleListNode &TupleListNode::operator=(TupleListNode &&another_node) {
    if (tuple_size != another_node.tuple_size) {
        throw std::invalid_argument("assigning a TupleListNode to another with a different size!");
    }
    if (this == &another_node) {
        return *this;
    }

    next = another_node.next;
    // since both data have the same size (tuple_size) then no need to delete old data and reallocate new space
    for (int i = 0; i < tuple_size; i++) {
        data[i] = another_node.data[i];
    }
    another_node.data = nullptr;
    return *this;
}

std::ostream &operator<<(std::ostream &os, const TupleListNode &node) {
    for (int i = 0; i < node.tuple_size; i++) {
        os << node.data[i];
        if (i != node.tuple_size - 1) {
            os << ' ';
        }
    }
    return os;
}
