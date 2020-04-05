#pragma once


template<class T>
struct LinkedListNode {
    T* valuePtr;
    LinkedListNode<T>* previous;

    LinkedListNode() {
        valuePtr = nullptr;
        previous = nullptr;
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
};

template<class T>
class LinkedList{
public:
    LinkedListNode<T>* last;
    unsigned int count;

    LinkedList(){
        count = 0;
        last = nullptr;
    }

    explicit LinkedList(T* value){
        last = new LinkedListNode<T>(value);
    }

    void push_right(T* value){
        auto* newNode = new LinkedListNode<T>(value);
        if(last != nullptr){
            newNode->previous = last;
        }
        last = newNode;
        count += 1;
    }

    void insert(unsigned int index, T* value){
        if(index < count){
            LinkedListNode<T>* currentNode = get(index);
            auto* newNode = new LinkedListNode<T>(value);
            newNode->previous = currentNode->previous;
            currentNode->previous = newNode;
            count += 1;
        }
    }

    LinkedListNode<T>* get(unsigned int index){
        auto* currentNode = last;
        auto invertedIndex = count - index - 1;
        for(auto i = 0; i < invertedIndex; i++)
            currentNode = currentNode->previous;
        return currentNode;
    }

    void remove(unsigned int index){
        if(index < count){
            if(count == 1)
                last = nullptr;
            else{
                auto* currentNode = last;
                auto* preCurrentNode = last;
                auto invertedIndex = count - index - 1;
                for(auto i = 0; i < invertedIndex; i++){
                    preCurrentNode = currentNode;
                    currentNode = currentNode->previous;
                }
                if(index == count - 1)
                    last = currentNode->previous;
                else
                    preCurrentNode->previous = currentNode->previous;
            }
            count -= 1;
        }
    }
};