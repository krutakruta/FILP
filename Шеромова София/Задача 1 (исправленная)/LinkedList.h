#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <iostream>
#include "Node.h"

template<typename T>
struct LinkedList {
private:
    size_t _size = 0;
    Node<T> *head;
    Node<T> *tail;

public:
    LinkedList() {
        head = tail = NULL;
        _size = 0;
    }

    ~LinkedList() = default;

    int size() {
        return _size;
    };

    void shift(T *value) {
        auto *node = new Node<T>(value, NULL, NULL);
        if (_size == 0) {
            head = tail = node;
        } else {
            head->prev = node;
            node->next = head;
            head = node;
        }
        _size++;
    }

    void push(T *value) {
        auto *node = new Node<T>(value, NULL, NULL);
        if (_size == 0) {
            head = tail = node;
        } else {
            tail->next = node;
            node->prev = head;
            tail = node;
        }
        _size++;
    }

    T *unshift() {
        if (_size == 0) {
            throw std::runtime_error("LinkedList does not contain elements.");
        }
        Node<T> *unshift_node = head;
        if (_size == 1) {
            head = tail = NULL;
        } else {
            head = unshift_node->next;
            head->prev = NULL;
        }
        _size--;
        return unshift_node->value;
    };

    T pop() {
        if (_size == 0) {
            throw std::runtime_error("LinkedList does not contain elements.");
        }
        Node<T> *pop_node = tail;
        if (_size == 0) {
            head = tail = NULL;
        } else {
            tail = pop_node->prev;
            tail->next = NULL;
        }
        _size--;
        return pop_node->value;
    };

    T *get(int index) {
        if (index < 0 || index >= _size) {
            throw std::runtime_error("Invalid index.");
        }
        int currentIndex = 0;
        Node<T> *currentNode = head;
        while (currentIndex < index) {
            currentIndex++;
            currentNode->next;
        }
        return currentNode->value;
    };
};

#endif
