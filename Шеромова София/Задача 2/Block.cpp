#include "Block.h"
#include "Consts.h"
#include <iostream>

Block::Block(void *memoryPtr, const int level, const size_t size) : memoryPtr(memoryPtr), level(level) {
    if (size > BLOCK_MIN_SIZE) {
        left = new Block(memoryPtr, level + 1, size / 2);
        right = new Block((int *) memoryPtr + size / 2, level + 1, size / 2);
        left->parent = this;
        right->parent = this;
    }
}

void *Block::allocate(int newLevel) {
    if (!isFree || newLevel < level || (isSplitted && newLevel == level)) {
        return nullptr;
    }

    if (!isSplitted && newLevel == level) {
        isFree = false;
        return memoryPtr;
    }

    isSplitted = true;
    const auto leftPtr = left->allocate(newLevel);
    return leftPtr != nullptr ? leftPtr : right->allocate(newLevel);
}

Block *Block::getBlock(void *ptr) {
    if (ptr == memoryPtr && !isFree) {
        return this;
    }

    if (left == nullptr) {
        return nullptr;
    }

    auto blockPtr = left->getBlock(ptr);
    if (blockPtr == nullptr) {
        blockPtr = right->getBlock(ptr);
    }
    return blockPtr;
}

void Block::free() {
    isFree = true;
    isSplitted = false;
    if (parent == nullptr) {
        return;
    }

    const auto buddyBlock = parent->left == this ? parent->right : parent->left;
    if (buddyBlock->isFree && !buddyBlock->isSplitted) {
        parent->free();
    }
}

void Block::printState() const {
    std::cout << std::endl;
    std::cout << " <<<< " << this << " >>>> " << std::endl;
    std::cout << "level: " << level << std::endl;
    std::cout << "parent: " << parent << std::endl;
    std::cout << "left: " << left << std::endl;
    std::cout << "right: " << right << std::endl;
    std::cout << "isFree: " << isFree << std::endl;
    std::cout << "isSplitted: " << isSplitted << std::endl;
    std::cout << std::endl;

    if (left != nullptr && (left->isSplitted || !left->isFree)) {
        left->printState();
    }
    if (right != nullptr && (right->isSplitted || !right->isFree)) {
        right->printState();
    }
}

void Block::printMemory() const {
    std::cout << "|";
    this->printMemory(this->isFree);
    std::cout << "|" << std::endl;
}

void Block::printMemory(bool parentBlockIsFree) const {
    const bool currentBlockIsFree = parentBlockIsFree && this->isFree;
    if (left == nullptr) {
        std::cout << (currentBlockIsFree ? "_" : "X");
        return;
    }
    left->printMemory(currentBlockIsFree);
    if (this->isSplitted) {
        std::cout << "|";
    } else {
        std::cout << "_";
    }
    right->printMemory(currentBlockIsFree);
}

Block::~Block() {
    delete left;
    delete right;
}