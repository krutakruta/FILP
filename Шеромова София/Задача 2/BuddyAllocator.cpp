#include "BuddyAllocator.h"
#include <cmath>

double getPowerOfTwo(const int number) {
    return pow(2, ceil(log2(number)));
}

int BuddyAllocator::getLevelBlock(const size_t sizeBlock) const {
    return (int) log2(memorySize / getPowerOfTwo(sizeBlock));
}

BuddyAllocator::BuddyAllocator(const size_t size) {
    memorySize = getPowerOfTwo(size);
    memory = malloc(memorySize);
    tree = new Block(memory, 0, memorySize);
}

void *BuddyAllocator::alloc(const size_t size) const {
    if (size > memorySize) {
        throw std::invalid_argument("Requested size is larger than allowed size.");
    }

    const auto ptr = tree->allocate(getLevelBlock(size));
    if (ptr == nullptr) {
        throw std::out_of_range("Not enough free memory.");
    }

    return ptr;
}

void BuddyAllocator::free(void *ptr) const {
    tree->getBlock(ptr)->free();
}

void BuddyAllocator::dump() const {
    tree->printMemory();
}

void BuddyAllocator::printMemoryTree() const {
    tree->printState();
}

BuddyAllocator::~BuddyAllocator() {
    delete tree;
    ::free(memory);
}