#include <iostream>
using namespace std;


struct Block {
    int level;
    bool free;

    union {
        struct {
            Block* next;
            Block* prev;
        };
        char data[1];
    };
};

class BuddyAllocator {
    char* memory;
    Block* freeBlocks[32];
    int maxLevel;
    size_t freeBlocksMask;
    const int fieldsSize = 8;

public:
    BuddyAllocator(size_t buffer) {
        maxLevel = log2(buffer);
        memory = (char*)malloc(buffer);
        Block* block = reinterpret_cast<Block*>(memory);
        block->level = maxLevel;
        addFreeBlock(block);
    }

    ~BuddyAllocator() {
        free(memory);
    }
    BuddyAllocator(const BuddyAllocator&other) {
        memory = other.memory; 
    }
    BuddyAllocator& operator=(const BuddyAllocator& other) = default;

    void* alloc(size_t size) {
        size_t mostSuitableSize = 1 << (bsr(size + fieldsSize - 1) + 1);
        size_t suitableLevelsMask = ~(mostSuitableSize - 1);
        size_t freeSuitableLevelsMask = suitableLevelsMask & freeBlocksMask;
        size_t smallestSuitableBlock = bsf(freeSuitableLevelsMask);
        Block* block = removeFreeBlock(smallestSuitableBlock);

        while (mostSuitableSize <= 1 << (block->level - 1)) {
            block->level--;
            Block* freeBlock = reinterpret_cast<Block*>(block->data - fieldsSize + (1 << block->level));
            freeBlock->level = block->level;
            addFreeBlock(freeBlock);
        }
        return block->data;
    }

    void dealloc(void* ptr) {
        Block* blockToFree = reinterpret_cast<Block*>((char*)ptr - fieldsSize);
        Block* mergedBlock = mergeWithBuddy(blockToFree);
        addFreeBlock(mergedBlock);
    }

    void dump() {
        cout << "Free blocks: " << endl;
        size_t levelsWithFreeBlocks = freeBlocksMask;

        while (levelsWithFreeBlocks != 0) {
            int freeBlockPos = bsr(levelsWithFreeBlocks);
            cout << "Level " << freeBlockPos << ": ";
            levelsWithFreeBlocks &= ~(1 << freeBlockPos);

            Block* freeBlock = freeBlocks[freeBlockPos];
            cout << freeBlock << " ";

            while (freeBlock->next != nullptr) {
                freeBlock = freeBlock->next;
                cout << freeBlock << " ";
            }
            cout << endl;
        }
        cout << endl;
    }

private:
    int bsr(int number) {
        int pos = 0;
        while (number >>= 1)
            pos++;
        return pos;
    }

    int bsf(int number) {
        int pos = 0;
        while ((number & 1) == 0) {
            number >>= 1;
            pos++;
        }
        return pos;
    }

    void addFreeBlock(Block* block) {
        block->free = true;
        block->prev = nullptr;

        if (freeBlocksMask & (1 << block->level)) {
            block->next = freeBlocks[block->level];
            freeBlocks[block->level]->prev = block;
        }
        else {
            block->next = nullptr;
            freeBlocksMask |= (1 << block->level);
        }
        freeBlocks[block->level] = block;
    }

    Block* mergeWithBuddy(Block* block) {
        while (true) {
            int blockSize = 1 << block->level;
            char* blockAddr = block->data - fieldsSize;
            int blockNumber = (blockAddr - memory) / blockSize;
            Block* buddy;

            if (block->level == maxLevel) {
                break;
            }
            if (blockNumber % 2 == 0) {
                char* buddyAddr = blockAddr + blockSize;
                buddy = reinterpret_cast<Block*>(buddyAddr);
            }
            else {
                char* buddyAddr = blockAddr - blockSize;
                buddy = reinterpret_cast<Block*>(buddyAddr);
                if (buddy->free && buddy->level == block->level) {
                    block = buddy;
                }
            }
            if (buddy->free && buddy->level == block->level) {
                removeBuddy(buddy);
                block->level++;
                block->free = true;
            }
            else {
                break;
            }
        }
        return block;
    }

    void removeBuddy(Block* buddy) {
        if (buddy->prev == nullptr && buddy->next == nullptr) {
            freeBlocksMask &= (~(1 << buddy->level));
        }
        if (buddy->prev != nullptr) {
            buddy->prev->next = buddy->next;
        }
        if (buddy->next != nullptr) {
            buddy->next->prev = buddy->prev;
        }
    }

    Block* removeFreeBlock(int level) {
        Block* block = freeBlocks[level];
        if (block->next == nullptr) {
            freeBlocksMask &= (~(1 << level));
        }
        else {
            block->next->prev = nullptr;
            freeBlocks[level] = block->next;
        }
        block->free = false;
        return block;
    }
};

int main()
{
    BuddyAllocator* allocator = new BuddyAllocator(256);
    allocator->dump();

    void *ptr1 = allocator->alloc(40);
    void *ptr2 = allocator->alloc(18);
    void *ptr3 = allocator->alloc(40);
    void *ptr4 = allocator->alloc(5);
    allocator->dump();

    allocator->dealloc(ptr1);
    allocator->dump();

    allocator->dealloc(ptr3);
    allocator->dump();

    allocator->dealloc(ptr4);
    allocator->dump();

    allocator->dealloc(ptr2);
    allocator->dump();

    return 0;
}