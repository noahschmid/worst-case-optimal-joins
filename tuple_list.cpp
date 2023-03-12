#include "tuple_list.h"

TupleListNode::TupleListNode(int tuple_size_arg, TupleListNode *nextNode) : tuple_size(tuple_size_arg),
                                                                            next(nextNode) {
    data = new int[tuple_size];
}

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

std::ostream &operator<<(std::ostream &os, const TupleListNode &node) {
    os << '(';
    for (int i = 0; i < node.tuple_size; i++) {
        os << node.data[i];
        if (i != node.tuple_size - 1) {
            os << ", ";
        }
    }
    os<< ')';
    return os;
}

TupleList::TupleList(int tuple_size) {
    head = new TupleListNode(tuple_size);
    tail = head;
}

void TupleList::append(TupleListNode *node) {
    tail->next = node;
    while (tail->next != nullptr) {
        tail = tail->next;
    }
}

TupleList::~TupleList() {
    TupleListNode *curr = head;
    TupleListNode *to_be_deleted;
    while (curr != nullptr) {
        to_be_deleted = curr;
        curr = curr->next;
        delete to_be_deleted;
    }
}

std::ostream &operator<<(std::ostream &os, const TupleList &list) {
    TupleListNode *curr = list.head->next;
    while (curr != nullptr) {
        os << (*curr);
        if (curr->next != nullptr) {
            os << " -> ";
        }
        curr = curr->next;
    }
    return os;
}