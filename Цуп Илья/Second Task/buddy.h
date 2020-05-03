#pragma once

#include <cmath>
#include "linked_list.h"

struct BuddyBlock {
    void* start;
    size_t size;

    BuddyBlock(void* blockStart, size_t blockSize){
        start = blockStart;
        size = blockSize;
    }
};

template<class T>
struct BuddyAllocator {
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    BuddyAllocator() : BuddyAllocator(1024 * 32)
    {
    }

    ~BuddyAllocator(){
        free(memoryBlockStart);
    }

    explicit BuddyAllocator(size_t kilobytes) {
        allocationSize = getAsPowerOfTwo(kilobytes) * 1024;
        memoryBlockStart = malloc(allocationSize);
        blocks = (LinkedList<BuddyBlock>*) malloc(sizeof(LinkedList<BuddyBlock>) * getListsOfBlocksCount());
        for(auto i = 0; i < getListsOfBlocksCount(); i++)
            blocks[i] = *new LinkedList<BuddyBlock>();
        blocks[getListsOfBlocksCount() - 1].push_right(new BuddyBlock(memoryBlockStart, allocationSize));
    }

    T* allocate(size_t memoryAmount) {
        auto appropriateIndex = getAppropriateSize(memoryAmount);
        auto currentIndex = appropriateIndex;
        BuddyBlock* allocatedBlock = nullptr;
        while (true) {
            if (currentIndex == appropriateIndex && blocks[currentIndex].count > 0) {
                allocatedBlock = blocks[currentIndex].get(0)->valuePtr;
                blocks[currentIndex].remove(0);
                return reinterpret_cast<T*>(allocatedBlock->start);
            }
            else if (currentIndex < getListsOfBlocksCount()) {
                if (blocks[currentIndex].count > 0) {
                    BuddyBlock* blockToRemove = blocks[currentIndex].get(0)->valuePtr;
                    blocks[currentIndex - 1].push_right(new BuddyBlock(blockToRemove->start, blockToRemove->size / 2));
                    blocks[currentIndex - 1].push_right(new BuddyBlock((void *)
                    ((char *)blockToRemove->start + blockToRemove->size / 2),
                            blockToRemove->size / 2));
                    blocks[currentIndex].remove(0);
                    currentIndex--;
                }
                else
                    currentIndex++;
            }
            else
                return nullptr;
        }
    }

    void deallocate(T* p, size_t memoryAmount) {
        auto index = getAppropriateSize(memoryAmount);
        blocks[index].push_right(new BuddyBlock((void*) p, getAsPowerOfTwo(memoryAmount)));
        sortBuddyBlocksByAddress(index);
        mergeAllBlocks();
    }

private:
    size_t allocationSize{};
    void* memoryBlockStart{};
    size_t block_min_size = getAsPowerOfTwo(sizeof(T));
    LinkedList<BuddyBlock>* blocks{};

    size_t getAsPowerOfTwo(size_t value){
        return pow(2, ceil(log2(value)));
    }

    size_t getAppropriateSize(size_t sizeKilobytes) {
        return ceil(std::max(log2(sizeKilobytes), log2(block_min_size)) - log2(block_min_size));
    }

    size_t getListsOfBlocksCount() {
        return log2(allocationSize) - log2(block_min_size) + 1;
    }

    void sortBuddyBlocksByAddress(size_t index) {
        LinkedList<BuddyBlock> unsorted = blocks[index];
        LinkedList<BuddyBlock> sorted = *(new LinkedList<BuddyBlock>());
        sorted.push_right(unsorted.get(0)->valuePtr);
        for (auto i = 1; i < unsorted.count; i++) {
            BuddyBlock* bubble = unsorted.get(i)->valuePtr;
            for (auto j = 0; i < sorted.count; j++) {
                if (bubble->start <= (sorted.get(j)->valuePtr->start)) {
                    sorted.insert(j, bubble);
                    break;
                }
                else if (j == sorted.count - 1 && bubble->start > (sorted.get(j)->valuePtr->start)){
                    sorted.push_right(bubble);
                    break;
                }
            }
        }
        blocks[index] = sorted;
    }

    int getBuddyBlockIndexToMerge(size_t index) {
        LinkedList<BuddyBlock> list = blocks[index];
        if (list.count > 1) {
            int blocksToMerge = -1;
            for (auto i = 0; i <= list.count; i++){
                auto* firstBuddy = list.get(i);
                auto* secondBuddy = list.get(i + 1);
                if(firstBuddy->valuePtr->start == (char*) secondBuddy->valuePtr->start - secondBuddy->valuePtr->size)
                    return i + 1;
            }
        }
        return -1;
    }

    void mergeBySize(size_t index) {
        int blockToMerge = getBuddyBlockIndexToMerge(index);
        while (blockToMerge != -1) {
            blocks[index].remove(blockToMerge);
            auto startOfBigBlock = blocks[index].get(blockToMerge);
            blocks[index].remove(blockToMerge); // Because blocks are adjacent
            blocks[index + 1].push_right(new BuddyBlock(startOfBigBlock->valuePtr->start, startOfBigBlock->valuePtr->size * 2));
            sortBuddyBlocksByAddress(index + 1);
            blockToMerge = getBuddyBlockIndexToMerge(index);
        }
    }

    void mergeAllBlocks() {
        for (auto i = 0; i < getListsOfBlocksCount(); ++i)
            mergeBySize(i);
    }
};