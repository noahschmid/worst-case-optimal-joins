#include "tuple_list.h"

TupleListNode::TupleListNode(TupleListNode *node): tuple_size(node->tuple_size) {
    data = new int[tuple_size];
    for(int i = 0; i < tuple_size; ++i)
        data[i] = node->data[i];
    next = node->next;
}

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
    os << ')';
    return os;
}

TupleList::TupleList() {
    head = new TupleListNode(0);
    tail = head;
}

TupleList::TupleList(const TupleList *list) {
    TupleListNode *cursor = new TupleListNode(list->head);
    
    head = cursor;

    while(cursor->next) {
        TupleListNode *next = new TupleListNode(cursor->next);
        cursor->next = next;
        cursor = next;
    }

    tail = cursor;
}

TupleList::TupleList(const Table &table) : TupleList() {
    for (const std::vector<int> &row: table.data) {
        this->append(new TupleListNode(row));
    }
}

void TupleList::append(TupleListNode *node) {
    tail->next = node;
    while (tail->next != nullptr) {
        tail = tail->next;
    }
}

void TupleList::merge(TupleList *list) {
    tail->next = list->head->next;
    tail = list->tail;
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
    if (list.empty()) {
        os << "[]";
        return os;
    }

    TupleListNode *curr = list.head->next;
    os << "[";
    while (curr != nullptr) {
        os << (*curr);
        if (curr->next != nullptr) {
            os << ", ";
        }
        curr = curr->next;
    }

    os << "]";
    
    return os;
}


TupleListNode *TupleList::pop_left() {
    if (empty()) {
        return nullptr;
    }

    TupleListNode *popped_node = head->next;
    head->next = popped_node->next;
    popped_node->next = nullptr;
    return popped_node;
}

int TupleList::length() {
    TupleListNode *cursor = head;
    int length = 0;
    while(cursor->next) {
        length++;
        cursor = cursor->next;
    }

    return length;
}