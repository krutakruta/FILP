#pragma once


template<class T>
struct LinkedListNode {
    T* valuePtr;
    LinkedListNode<T>* previous;

    LinkedListNode() {
        valuePtr = nullptr;
        previous = nullptr;
    }

    ~LinkedListNode() {
        delete valuePtr;
        if (previous != nullptr)
            delete previous;
    }

    LinkedListNode& operator=(const LinkedListNode& x) {
        this->previous = x->prev;
        this->valuePtr = x->valuePtr;
        return *this;
    }

    explicit LinkedListNode(T* value) {
        valuePtr = value;
        previous = nullptr;
    }

    LinkedListNode<T>* push_back(T* value) {
        auto* newNode = new LinkedListNode<T>(value);
        newNode->previous = this;
        return newNode;
    }
};

template<class T>
class LinkedList{
public:
    LinkedListNode<T>* last;

    LinkedList(){
        last = nullptr;
    }

    explicit LinkedList(T* value){
        last = new LinkedListNode<T>(value);
    }

    ~LinkedList(){
        delete last;
    }

    void push_back(T* value){
        auto* newNode = new LinkedListNode<T>(value);
        if(last != nullptr)
            newNode->previous = last;
        last = newNode;
    }

    void remove_last(){
        if(last != nullptr){
            last = last->previous;
        }
    }
};