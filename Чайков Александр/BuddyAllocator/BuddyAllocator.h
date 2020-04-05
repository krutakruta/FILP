#include <vector>
#include <list>
#include <cmath>

using namespace std;


struct Block {
    char* begin;
    int size;
    bool used;
    Block(int iSize)
    {
        begin = (char*)malloc(iSize);
        size = iSize;
        used = false;
    }
    Block(int size, char* begin)
    {
        this->size = size;
        this->begin = begin;
        used = false;
    }
};

int int_pow(int base, int exp)
{
    int result = 1;
    while (exp)
    {
        if (exp % 2)
            result *= base;
        exp /= 2;
        base *= base;
    }
    return result;
}

template<typename T>

struct BuddyAllocator {
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;
    const int blockSize = 64;
    const int maxOrder = 8;
    const int countBlocks = int_pow(2, maxOrder);
    vector<list<Block>> memory;
    template< class U >
    struct rebind {
        typedef BuddyAllocator<U> other;
    };

    void printMemory() const
    {
        for(int i = 0;i < memory.size();i++)
        {
            cout << "order " << i << " size " << memory[i].size() << endl;
            for(auto it = memory[i].begin();it != memory[i].end();it++)
            {
                cout << it->size << " used " << it->used << endl;
            }
        }
    }


    Block find(int order, bool isDivide)
    {
        for(auto it = memory[order].begin();it != memory[order].end();it++) {
            if (!it->used) {
                if(isDivide) {
                    Block res = *it;
                    memory[order].erase(it);
                    return res;
                }
                else{
                    (*it).used = true;
                    return *it;
                }
            }
        }
        Block biggerBlock = find(order + 1, true);
        int newSize = biggerBlock.size / 2;
        Block* firstBlock = &Block(newSize, biggerBlock.begin);
        Block* secondBlock = &Block(newSize, biggerBlock.begin + newSize);
        if(!isDivide) {
            firstBlock->used = true;
            memory[order].push_back(*firstBlock);
        }
        memory[order].push_back(*secondBlock);
        return *firstBlock;
    }

    int calculateOrder(int n)
    {
        int requiredSize = n * sizeof(T);
        int countBlocks = ceil((double)requiredSize / blockSize);
        return ceil(log2(countBlocks));
    }

    BuddyAllocator() noexcept
    {
        //cout << "construct" << this << endl;
        for(int i = 0;i < maxOrder + 1;i++)
        {
            list<Block> currentList;
            memory.push_back(currentList);
        }
        memory[maxOrder].push_back(Block(blockSize * countBlocks));

    }
    ~BuddyAllocator() noexcept
    {
        //cout << "destruct" << this  <<endl;
        memory.clear();
    }

    BuddyAllocator(const BuddyAllocator& other) noexcept
    {
    }

    template< class U >
    explicit BuddyAllocator(const BuddyAllocator<U>& other) noexcept
    {
        //cout << "copy" << endl;
        for(int i = 0;i < maxOrder + 1;i++)
        {
            list<Block> currentList;
            memory.push_back(currentList);
        }
        memory[maxOrder].push_back(Block(blockSize * countBlocks));
    }

    T* allocate(size_t n)
    {
        //cout << this << " allocate " << n * sizeof(T) << " bytes " << n << " objects " << endl;
        int order = calculateOrder(n);
        Block* requiredBlock = &find(order, false);
        //printMemory();
        return (T*)requiredBlock->begin;
    }

    void deallocate(T* p, size_t n)
    {
        //cout << " deallocate " << n << " objects " << sizeof(T)*n << "bytes" << endl;
        int order = calculateOrder(n);
        for(auto it = memory[order].begin();it != memory[order].end();it++)
        {
            if(p == (T*)it->begin){
                it->used = false;
            }
        }
    }
};
