#ifndef NODE_H
#define NODE_H

template<typename T>
struct Node {
    Node *next;
    Node *prev;
    T *value;

    explicit Node(T *value, Node *prev, Node *next) : value(value), next(next), prev(prev) {}

    ~Node() {
        delete value;
        value = next = prev = 0;
    }
};

#endif
