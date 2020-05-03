using namespace std;

template<class T>
struct buffer_block {

private:
    bool is_dealloced = false;
    void deleteMemoryPtr()
    {
        if (is_dealloced) { cout << "has already dealloced\n"; }
        else {
            delete[] memory_ptr;
            is_dealloced= true;
        }
    }
public:
    using value_type = T;
    std::size_t block_size;
    bool is_free = true;
    buffer_block *prev = nullptr;
    buffer_block *next = nullptr;
    char *memory_ptr;
    int startIdx;

    buffer_block<T>(size_t size) {
        memory_ptr = new char[size];
        block_size = size;
        cout << "create buffer_block of " << block_size << " size\n";
    }

    ~buffer_block<T>() {
        deleteMemoryPtr();
        cout << "del buffer_block of " << this->block_size << " size\n";
    }

    //buffer_block operator=(const buffer_block &c) = delete;

    T *alloc(size_t n) {
        if (n <= block_size && is_free) {
            cout << "can alloc\n";
            T *_Ptr = reinterpret_cast<T *>(memory_ptr);
            is_free = false;
            return _Ptr;
        } else {
            throw std::bad_alloc();
        }
    }

    void dealloc(T *p, size_t n) {
        cout << "dealloc\n";
        deleteMemoryPtr();
    }

    void connectTo(buffer_block *otherBlock) {

        cout << "connect " << this << " to " << otherBlock << "\n";
        this->next = otherBlock;
        otherBlock->prev = this;
        cout << "now " << this << "==" << otherBlock->prev << " connected to " << this->next << "==" << otherBlock
             << "\n";

    }

    void splitBlock() {
        cout << "split block of " << block_size << " size;\n";
        size_t newSize = this->block_size / 2;
        auto newBlockA = new buffer_block(newSize);
        auto newBlockB = new buffer_block(newSize);
        auto temp = this->prev;
        if (temp != nullptr)
            temp->connectTo(newBlockA);
        newBlockA->connectTo(newBlockB);
        if (this->next != nullptr)
            newBlockB->connectTo(this->next);
        newBlockA->startIdx = this->startIdx;
        newBlockB->startIdx = this->startIdx + newSize;
        cout << "splitted\n";
        delete this;
    }

    buffer_block *mergeBlock(buffer_block *otherBlock) {
        cout << "merge " << this << " and " << otherBlock << "\n";
        auto last_prev = this->prev;
        auto last_next = otherBlock->next;
        size_t new_size = this->block_size * 2;
        delete this;
        delete otherBlock;
        auto new_block = new buffer_block(new_size);
        if (last_prev != nullptr)
            last_prev->connectTo(new_block);
        if (last_next != nullptr)
            new_block->connectTo(last_next);
        return new_block;
    }

    void mergeBlockIfNeeded() {
        size_t size = this->block_size;
        size_t offset = this->startIdx / size;
        auto maybe_buddy = (offset % 2 == 0) ? this->next : this->prev;
        auto delta = (offset % 2 == 0) ? -1 : 1;
        if (maybe_buddy != nullptr && maybe_buddy->is_free && maybe_buddy->block_size == size &&
            maybe_buddy->startIdx / size + delta == offset) {
            buffer_block *tmp = this->mergeBlock(maybe_buddy);
            tmp->mergeBlockIfNeeded();
        }
    }


};