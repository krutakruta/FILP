#ifndef BUDDYALLOCATOR_H
#define BUDDYALLOCATOR_H

#include<iostream>
#include "Block.h"

class BuddyAllocator {
    Block *tree;
    void *memory;
    size_t memorySize;

private:
    int getLevelBlock(size_t sizeBlock) const;

public:
    explicit BuddyAllocator(size_t size);

    void *alloc(size_t size) const;

    void free(void *ptr) const;

    void dump() const;

    void printMemoryTree() const;

    ~BuddyAllocator();
};

#endif