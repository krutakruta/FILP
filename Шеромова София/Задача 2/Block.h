#ifndef BLOCK_H
#define BLOCK_H

#include<iostream>

struct Block {
private:
    void printMemory(bool parentBlockIsFree) const;

public:
    Block *parent = nullptr;
    Block *left = nullptr;
    Block *right = nullptr;
    void *memoryPtr;
    int level;
    bool isFree = true;
    bool isSplitted = false;

    Block(void *memoryPtr, int level, size_t size);

    void *allocate(int newLevel);

    Block *getBlock(void *ptr);

    void free();

    void printState() const;

    void printMemory() const;

    ~Block();
};

#endif