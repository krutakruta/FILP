using namespace std;

struct AllocBuff {
    AllocBuff* prev;
    size_t buffSize;
    void* buffPtr;
    size_t bufferPos;
    AllocBuff() = default;
    AllocBuff(AllocBuff* nPrev, size_t nBuffSize, void* nBuffPtr, size_t nBufferPos){
        prev = nPrev;
        buffSize = nBuffSize;
        buffPtr = nBuffPtr;
        bufferPos = nBufferPos;
    }
};



template<typename T>
struct mallocator {
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;
    const size_t size = 1000;
    const size_t bytesSize = size* sizeof(T);
    template< class U >
    struct rebind {
        typedef mallocator<U> other;
    };

    mallocator() noexcept
    {
    }
    ~mallocator() noexcept
    {
        //cout << "destruct" << endl;
        while(pAllocBuff != nullptr){
            auto next = pAllocBuff->prev;
            free(pAllocBuff);
            pAllocBuff = next;
        }
    }

    mallocator(const mallocator& other) noexcept
    {
    }

    template< class U >
    mallocator(const mallocator<U>& other) noexcept
    {
    }

    T* allocate(size_t n)
    {
        //cout << "allocate" << " " << n* sizeof(T) << endl;
        if(pAllocBuff->bufferPos + n > size){
            AllocBuff* nextBuffer = new AllocBuff(nullptr, size, malloc(bytesSize), 0);
            nextBuffer->prev = pAllocBuff;
            pAllocBuff = nextBuffer;
        }
        value_type* memory = (value_type*)((T*)pAllocBuff->buffPtr + pAllocBuff->bufferPos);
        pAllocBuff->bufferPos += n;
        return memory;
    }

    void deallocate(T* p, size_t n)
    {
        //cout << "deallocate" << endl;
    }

private:
    AllocBuff* pAllocBuff = new AllocBuff(nullptr, size, malloc(bytesSize), 0);
};