#include <iostream>
using namespace std;

class BuddyBlock {
public:
    BuddyBlock* parent;

    BuddyBlock* previous;
    BuddyBlock* next;

    BuddyBlock* first;
    BuddyBlock* second;

    size_t size;
    int* ptr;
    bool empty;

    BuddyBlock(BuddyBlock* _parent, size_t _size) {
        size = _size;
        parent = _parent;

        empty = true;
        ptr = (int*)malloc(size);

        previous = nullptr;
        next = nullptr;

        first = nullptr;
        second = nullptr;
    }

    ~BuddyBlock(){
        empty = true;
        parent = nullptr;

        previous = nullptr;
        next = nullptr;

        first = nullptr;
        second = nullptr;
        
        free(ptr);
    }

    void merge() {
        previous = first->previous;
        if (previous != nullptr)
            previous->next = this;

        next = second->next;
        if(next != nullptr)
            next->previous = this;

        first->~BuddyBlock();
        second->~BuddyBlock();
    }

    void split() {
        empty = false;
        first = new BuddyBlock(this, size / 2);
        second = new BuddyBlock(this, size / 2);

        
        if (previous != nullptr) 
            previous->next = first;
        first->previous = previous;
        first->next = second;
        second->previous = first;
        second->next = next;
        if (next != nullptr)
            next->previous = second;
        next = nullptr;
        previous = nullptr;
    }

    void dump() {
        cout << "addr: " << this << " size:" << size << " status: " << (empty ? "empty" : "occupied") << '\n';
    }
};


class BuddyAllocator {
public:
    BuddyBlock* first;
    size_t size;

    BuddyAllocator(size_t _size) {
        size = 1 << (int)ceil(log2(_size));
        first = new BuddyBlock(nullptr, size);
    }

    int* allocate(size_t _size) {
        BuddyBlock* current = first;

        if (first == nullptr)
            return NULL;

        size_t blockSize = 1 << (int)ceil(log2(_size));

        while (!current->empty || current->size!=blockSize)
        {
            if (current->empty && (blockSize < current->size)) {
                current->split();

                if (current == first)
                    first = current->first;

                current = current->first;
            }
            else {
                current = current->next;
            }
            if (current == nullptr)
                return NULL;
        }

        current->empty = false;
        return current->ptr;
    }


    void deallocate(int* p) {

        BuddyBlock* current = first;
        while (current->ptr!=p)
            current = current->next;

        current->empty = true;
        current = current->parent;

        while (current != nullptr && current->first->empty && current->second->empty) {
            if (current->first->empty && current->second->empty) {
                if (current->first == first)
                    first = current;
                current->merge();
            }
            current->empty = true;
            current = current->parent;
        }
    }

    void dump() {
        BuddyBlock* current = first;

        while (current!=nullptr)
        {
            current->dump();
            current = current->next;
        }
    }
};


int main()
{
    cout << "creating allocator size: " << 1024 << '\n';
    BuddyAllocator* alloc = new BuddyAllocator(1024);
    alloc->dump();

    cout << "\nallocating 32\n";
    int* a = alloc->allocate(32);
    alloc->dump();

    cout << "\nallocating 64\n";
    int* b = alloc->allocate(64);
    alloc->dump();

    cout << "\nallocating 60\n";
    int* c = alloc->allocate(60);
    alloc->dump();

    cout << "\nallocating 150\n";
    int* d = alloc->allocate(150);
    alloc->dump();

    cout << "\ndeallocating 64\n";
    alloc->deallocate(b);
    alloc->dump();

    cout << "\ndeallocating 32\n";
    alloc->deallocate(a);
    alloc->dump();

    cout << "\nallocating 100 and 100\n";
    int* e = alloc->allocate(100);
    int* f = alloc->allocate(100);
    alloc->dump();
}