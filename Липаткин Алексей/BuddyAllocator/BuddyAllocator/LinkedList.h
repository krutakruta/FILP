struct listBlock {
    void* address;
    listBlock* next;
};

class LinkedList {
public:
    LinkedList(int size) {
        blockSizeKb = size;
        head = NULL;
        tail = NULL;
        length = 0;
    }

    LinkedList() { LinkedList(DEFAULT_BLOCK_SIZE); }
    int getLength() { return length; }
    int getBlockSize() { return blockSizeKb; }

    listBlock* get(unsigned int position) {
        listBlock* current = new listBlock;
        current = head;
        if (position > 0) {
            for (int i = 0; i < position; i++) {
                current = current->next;
            }
        }
        return current;
    }

    void add(void* address) {
        listBlock* newBlock = new listBlock;
        newBlock->address = address;
        newBlock->next = NULL;
        if (length == 0) {
            head = newBlock;
            tail = newBlock;
        }
        else {
            newBlock->next = head;
            head = newBlock;
        }
        length++;
    }

    void push(void* address) {
        listBlock* newBlock = new listBlock;
        newBlock->address = address;
        newBlock->next = NULL;
        if (length == 0) {
            head = newBlock;
            tail = newBlock;
        }
        else {
            listBlock* secondLastBlock = get((length == 1) ? 0 : length - 2);
            secondLastBlock->next = newBlock;
            tail = newBlock;
        }
        length++;
    }

    void remove(unsigned int position) {
        if (length != 0 && position < length) {
            listBlock* removeBlock = new listBlock;
            removeBlock = get(position);
            if (length == 1) {
                head = NULL;
                tail = NULL;
            }
            else {
                if (position == 0) {
                    head = removeBlock->next;
                }
                else {
                    listBlock* blockBefore = new listBlock;
                    blockBefore = get(position - 1);
                    blockBefore->next = removeBlock->next;
                    if (position == length - 1) {
                        tail = blockBefore;
                    }
                }
            }
            delete removeBlock;
            length--;
        }
    }

private:
    listBlock* head;
    listBlock* tail;
    size_t blockSizeKb;
    size_t length;
};
