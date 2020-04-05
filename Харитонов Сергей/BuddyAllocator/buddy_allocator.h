//
// Created by ХаритоновСергейЕвген on 31.03.2020.
//

#ifndef ALLOCATOR_BUDDY_ALLOCATOR_H
#define ALLOCATOR_BUDDY_ALLOCATOR_H

#include <iostream>

struct AllocatedBlock {
    AllocatedBlock(size_t level, size_t number) {
        this->level = level;
        this->number = number;
    }

    size_t getSize() {
        return 1 << level;
    }

    size_t getOffset() {
        return number * getSize();
    }

    size_t level;
    size_t number;
    AllocatedBlock* next{};
    AllocatedBlock* prev{};
};

using namespace std;

template <class T>
struct BuddyAllocator {
    typedef T value_type;
    typedef T* pointer;
    typedef size_t size_type;

    BuddyAllocator() {
        initialize(20);
    }

    explicit BuddyAllocator(size_t maxDepth) {
        initialize(maxDepth);
    }

    pointer allocate(size_t size) {
        size *= type_size;
        auto level = getLevelBySize(size);
        auto block = takeBlock(level);
        if (block == nullptr){
            if (nextAllocator == nullptr)
                nextAllocator = new BuddyAllocator(maxDepth);

            return nextAllocator->allocate(size);
        }

        void* result = ptr + block->getOffset();

        return reinterpret_cast<pointer>(result);
    }

    void deallocate(pointer p, size_t size) {
        size *= type_size;
        auto level = getLevelBySize(size);
        auto offset = getOffset(p);
        auto number = offset / (1 << level);
        auto block = AllocatedBlock(level, number);
        block.next = blocks[level];
        blocks[level] = &block;
    }

    ~BuddyAllocator() {
        delete nextAllocator;
        free(ptr);
        delete blocks;
    }
private:
    size_t baseSize;
    size_t maxDepth;
    size_t type_size;
    void* ptr;
    AllocatedBlock** blocks;
    BuddyAllocator* nextAllocator;

    size_t getLevelBySize(size_t size){
        size_t level = 0;
        while (size > 0){
            size /= 2;
            level++;
        }
        return level;
    }

    void initialize(size_t maxDepth) {
        this->maxDepth = maxDepth;
        baseSize = 1 << maxDepth;
        ptr = malloc(baseSize);

        auto p1 = (pointer)ptr;
        auto p2 = p1 + 1;
        type_size = (size_t)p2 - (size_t)p1;

        blocks = new AllocatedBlock*[maxDepth];
        for (size_t i =0; i < maxDepth; i++)
            blocks[i] = nullptr;
        blocks[maxDepth - 1] = new AllocatedBlock(20, 0);
        nextAllocator = nullptr;
    }

    AllocatedBlock* takeBlock(size_t level) {
        if (level == maxDepth)
            return nullptr;

        if (blocks[level] != nullptr) {
            AllocatedBlock* current = blocks[level];
            blocks[level] = current->next;
            return current;
        }

        AllocatedBlock* block = takeBlock(level + 1);
        auto* returnedBlock = new AllocatedBlock(level, block->number * 2);
        block = new AllocatedBlock(level, block->number * 2  + 1);
        blocks[level] = block;

        return returnedBlock;
    }

    size_t getOffset(pointer receivedPointer){
        return (size_t)receivedPointer - (size_t)ptr;
    }
};

#endif //ALLOCATOR_BUDDY_ALLOCATOR_H
