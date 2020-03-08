#pragma once

#include "linked_list.h"
#include <iostream>

const size_t buffer_size = 1024 * 1024;



template <class T>
struct MemoryChunk{
    void* chunkPtr;
    size_t size;
    size_t ptr{};

    explicit MemoryChunk(void* ptr){
        chunkPtr = ptr;
        size = buffer_size;
    }

    ~MemoryChunk(){
        chunkPtr = nullptr;
    }

    bool can_allocate(size_t n){
        return ptr + sizeof(T) * n < buffer_size;
    }

    void* allocate(size_t n){
        void* allocated_pointer = chunkPtr + ptr;
        ptr += sizeof(T) * n;
        return allocated_pointer;
    }
};

template <class T>
struct CustomAllocator {
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    CustomAllocator() {
        chunks = *new LinkedList<MemoryChunk<T>>();
    }

    ~CustomAllocator(){
        LinkedListNode<MemoryChunk<T>>* current_chunk = chunks.last;
        while(current_chunk != nullptr){
            free(&(current_chunk->valuePtr->chunkPtr));
        }
    }


    T* allocate(size_type n) {
        if(chunks.last == nullptr ||
        !chunks.last->valuePtr->can_allocate(n)) //It doesn't work if n is bigger than buffer_size / sizeof(T)
            chunks.push_back(new MemoryChunk<T>(malloc(buffer_size)));
        return reinterpret_cast<T*>(chunks.last->valuePtr->allocate(n));
    }

    void deallocate(T* p, size_t size) {
        //free(chunks.last->valuePtr.chunkPtr);
        //chunks.remove_last();
        //this removes last buffer
    }

private:
    LinkedList<MemoryChunk<T>> chunks;
};