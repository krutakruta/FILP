#include <iostream>
#include "LinkedList.h"

using namespace std;

#define ALLOCATOR_SIZE 1024
#define BLOCK_MIN_SIZE 64

struct buddyBlock {
    char* start;
    size_t size;
};

class BuddyAllocator {
public:
    BuddyAllocator(size_t sizeKilobytes) {
        size = sizeKilobytes * 1000 * sizeof(char);
        memory = (char*)malloc(size);
    }

    BuddyAllocator() { BuddyAllocator(1024); }
    size_t getSize() { return size; }
    size_t getSizeKilobytes() { return size / 1000; }
    char* getMemoryPtr() { return memory; }

    buddyBlock* allocate(size_t size) {
        int index = getListNumber(size);
        bool found = false;
        listBlock* allocateBlock = new listBlock;
        while (!found) {
            if (blocks[index].getLength() > 0) {
                allocateBlock = blocks[index].get(0);
                blocks[index].remove(0);
                found = true;
            }
            else if (index < getListsSize()) {
                index++;
                if (blocks[index].getLength() > 0) {
                    listBlock* removeBlock = new listBlock;
                    removeBlock = blocks[index].get(0);
                    blocks[index - 1].add(((char*)removeBlock->address + (blocks[index - 1].getBlockSize() * 1000)));
                    blocks[index - 1].add(removeBlock->address);
                    blocks[index].remove(0);
                    index = getListNumber(size);
                }
            }
            else {
                break;
            }
        }
        if (found) {
            buddyBlock* foundBlock = new buddyBlock;
            foundBlock->start = (char*)allocateBlock->address;
            foundBlock->size = blocks[index].getBlockSize();
            return foundBlock;
        }
        else {
            return NULL;
        }
    }

    void deallocate(buddyBlock* ptr) {
        auto index = getListNumber(ptr->size);
        blocks[index].add(ptr->start);
        sortList(index);
        mergeAll();
    }
private:
    size_t size;
    char* memory;
    LinkedList* blocks;

    size_t getListNumber(size_t sizeKilobytes) { return ceil(log2(sizeKilobytes) - log2(BLOCK_MIN_SIZE)); }
    size_t getListsSize() { return getListNumber(getSizeKilobytes()) + 1; }
    
    void sortList(size_t index) {
        LinkedList unsorted = blocks[index];
        LinkedList sorted = LinkedList(unsorted.getBlockSize());
        sorted.add(unsorted.get(0)->address);
        for (auto i = 1; i < unsorted.getLength(); ++i) {
            void* insert = unsorted.get(i)->address;
            for (auto j = 1; i < sorted.getLength(); ++j) {
                if (insert <= (sorted.get(j)->address)) {
                    sorted.add(insert);
                    break;
                }
                else if (j == sorted.getLength() - 1 && insert > (sorted.get(j)->address)) {
                    sorted.push(insert);
                    break;
                }
            }
        }
        blocks[index] = sorted;
    }

    int* getBuddies(size_t index) {
        LinkedList list = blocks[index];
        if (list.getLength() > 1) {
            int* buddies = (int*)malloc(sizeof(int) * 2);
            int blockSize = list.getBlockSize() * 1000;
            for (auto i = 0; i <= list.getLength(); ++i) {
                if (list.get(i)->address == (char*)list.get(i+1)->address - blockSize) {
                    buddies[0] = i;
                    buddies[1] = i + 1;
                    return buddies;
                }
            }
        }
        return NULL;
    }

    void merge(size_t index) {
        int* buddies = getBuddies(index);
        while (buddies != NULL) {
            void* start = blocks[index].get(buddies[0])->address;
            blocks[index].remove(buddies[1]);
            blocks[index].remove(buddies[0]);
            blocks[index + 1].add(start);
            sortList(index + 1);
            buddies = getBuddies(index);
        }
    }
    
    void mergeAll() {
        for (auto i = 0; i < getListsSize(); ++i) {
            merge(i);
        }
    }
};

