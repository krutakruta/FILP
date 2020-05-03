#include <iostream>
#include "BuddyAllocator.h"

int main() {
    auto allocator = new BuddyAllocator(1000);
    allocator->dump();

    auto a = allocator->alloc(500);
    allocator->dump();

    auto b = allocator->alloc(250);
    allocator->dump();

    auto c = allocator->alloc(100);
    allocator->dump();

    allocator->free(a);
    allocator->dump();

    allocator->free(b);
    allocator->dump();

    auto d = allocator->alloc(20);
    allocator->dump();

    allocator->free(c);
    allocator->dump();

    allocator->free(d);
    allocator->dump();

    allocator->printMemoryTree();

    delete allocator;
    return 0;
}