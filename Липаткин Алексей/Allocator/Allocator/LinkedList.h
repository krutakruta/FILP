#include <iostream>
using namespace std;

struct Node {
    char* value;
    Node* prev;
    Node* next;
};

class LinkedList {
    Node* head;
    Node* tail;
    int count;

public:
    LinkedList() {
        head = tail = NULL;
        count = 0;
    };

    LinkedList(char* value) {
        Node* node = new Node;
        node->value = value;
        node->prev = node->next = NULL;
        head = tail = node;
        count = 1;
    };

    int GetCount() {
        return count;
    };

    char* PopRightElement() {
        if (count == 0)
            return NULL;
        if (count == 1) {
            char* value = head->value;
            head = tail = NULL;
            count--;
            return value;
        }
        count--;
        Node* pop_node = tail;
        tail = pop_node->prev;
        return pop_node->value;
    };

    char* PopLeftElement() {
        if (count == 0)
            return NULL;
        if (count == 1) {
            char* value = head->value;
            head = tail = NULL;
            count--;
            return value;
        }
        count--;
        Node* pop_node = head;
        head = pop_node->next;
        return pop_node->value;
    };

    void AddElementRight(char* value) {
        Node* node = new Node;
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

    void AddElementLeft(char* value) {
        Node* node = new Node;
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

    char* GetElement(int pos) {
        if (pos >= count or pos < 0) {
            return NULL;
        }
        if (pos > count / 2) {
            int currentIndex = count - 1;
            Node* currentNode = tail;
            while (pos < currentIndex) {
                currentIndex--;
                currentNode->prev;
            }
            return currentNode->value;
        }
        int currentIndex = 0;
        Node* currentNode = head;
        while (pos > currentIndex) {
            currentIndex++;
            currentNode->next;
        }
        return currentNode->value;
    };
};

class Stack {
    LinkedList* elements;

public:
    Stack() {
        elements = new LinkedList;
    };
    int GetCount() {
        return elements->GetCount();
    };
    char* Pop() {
        return elements->PopRightElement();
    };
    void Push(char* value) {
        elements->AddElementRight(value);
    };
};

class Queue {
    LinkedList* elements;

public:
    Queue() { elements = new LinkedList; };
    int GetCount() { return elements->GetCount(); };
    char* GetElement() { return elements->PopLeftElement(); };
    void AddElement(char* value) { elements->AddElementRight(value); };
};
