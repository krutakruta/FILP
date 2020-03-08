#include <iostream>
using namespace std;

template <class T>
struct Node {
    T value;
    Node* prev;
    Node* next;
};

template <class T>
class LinkedList {
    Node<T>* head;
    Node<T>* tail;
    int count;

public:
    LinkedList() {
        head = tail = NULL;
        count = 0;
    };

    LinkedList(T value) {
        Node<T>* node = new Node<T>;
        node->value = value;
        node->prev = node->next = NULL;
        head = tail = node;
        count = 1;
    };

    int getCount() {
        return count;
    };

    T popRightElement() {
        if (count == 1) {
            T value = head->value;
            head = tail = NULL;
            count--;
            return value;
        }
        count--;
        Node<T>* pop_node = tail;
        tail = pop_node->prev;
        return pop_node->value;
    };

    T popLeftElement() {
        if (count == 1) {
            T value = head->value;
            head = tail = NULL;
            count--;
            return value;
        }
        count--;
        Node<T>* pop_node = head;
        head = pop_node->next;
        return pop_node->value;
    };

    void addElementRight(T value) {
        Node<T>* node = new Node<T>;
        node->value = value;
        node->prev = tail;
        node->next = NULL;
        if (tail != NULL)
            tail->next = node;
        if (count == 0)
            head = tail = node;
        tail = node;
        count++;
    };

    void addElementLeft(T value) {
        Node<T>* node = new Node<T>;
        node->value = value;
        node->prev = NULL;
        node->next = head;
        if (head != NULL)
            head->prev = node;
        if (count == 0)
            head = tail = node;
        head = node;
        count++;
    };

    T getElement(int pos) {
        if (pos >= count or pos < 0) {
            return NULL;
        }
        if (pos > count / 2) {
            int currentIndex = count - 1;
            Node<T>* currentNode = tail;
            while (pos < currentIndex) {
                currentIndex--;
                currentNode->prev;
            }
            return currentNode->value;
        }
        int currentIndex = 0;
        Node<T>* currentNode = head;
        while (pos > currentIndex) {
            currentIndex++;
            currentNode->next;
        }
        return currentNode->value;
    };
};

template <class T>
class Stack {
    LinkedList<T>* elements;

public:
    Stack() {
        elements = new LinkedList<T>;
    };
    int getCount() {
        return elements->getCount();
    };
    char* pop() {
        return elements->popRightElement();
    };
    void push(char* value) {
        elements->addElementRight(value);
    };
};

template <class T>
class Queue {
    LinkedList<T>* elements;

public:
    Queue() { elements = new LinkedList<T>; };
    int getCount() { return elements->getCount(); };
    char* getElement() { return elements->popLeftElement(); };
    void addElement(char* value) { elements->addElementRight(value); };
};
