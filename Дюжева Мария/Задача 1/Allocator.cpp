#include <windows.h>
#include <iostream>
#include <map>
#include <string>
#include <time.h>
#include <vector>
#include <algorithm>
using namespace std;

struct Buffer {
    const size_t size = 1024 * 1024;
    size_t ptr;
    char *buffer = nullptr;
    Buffer *next;

    Buffer() { 
        buffer = new char[size];
        ptr = 0;
        next = NULL;
    }
    ~Buffer() {
        if (buffer != nullptr) {
            delete[] buffer;
        }
    }
};

struct Memory {
    Buffer* current;
    Buffer* head;

    Memory() { 
        current = new Buffer();
        head = current;
    }
    ~Memory() {
        while (head != NULL) {
            Buffer* oldBuffer = head;
            head = head->next;
            delete oldBuffer;
        }
    }

    void* getMemory(size_t number, size_t size) {
        if (current->ptr + size * number > current->size) {
            grow();
        }
        void* newPtr = current->buffer + current->ptr;
        current->ptr += size * number;
        return newPtr;
    }

    void grow() {
        Buffer *newBuffer = new Buffer();
        current->next = newBuffer;
        current = newBuffer;
    }
};

template <class T>
struct MyAllocator {
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    Memory* memory;
    MyAllocator() { 
        memory = new Memory(); 
    }
    template <class U> 
    MyAllocator(const MyAllocator<U>& other) noexcept { memory = other.memory; }

    pointer allocate(size_t n) {
        return reinterpret_cast<T*>(memory->getMemory(n, sizeof(T)));
    }

    MyAllocator& operator=(const MyAllocator& x) = default;

    ~MyAllocator() { }

    void deallocate(T* p, size_t n) { }

    void construct(pointer p, const T& value) {
        new (p) T(value);
    }
    void destruct(pointer p) {
        p->~T();
    }
    size_type max_size() {
        return allocator<T>::max_size();
    }
};

struct CharCompare {
    bool operator()(const char *x, const char *y) const {
        while (*x > 0 && *y > 0 && isalpha(*x) && isalpha(*y)) {
            if (tolower(*x) < tolower(*y))
                return true;
            if (tolower(*x) > tolower(*y))
                return false;
            x++, y++;
        }
        if (*x > 0 && *y > 0)
            return isalpha(*x) < isalpha(*y);
        return false;
    }
};

map<string, size_t> getMapWithStdAlloc(char* text) {
    map<string, size_t> map;
    string word;
    for (unsigned long i = 0; text[i] != '\0'; i++) {
        if (text[i] > 0 && isalpha(text[i])) {
            word += tolower(text[i]);
        }
        else {
            if (word != "") {
                map[word]++;
                word = "";
            }
        }
    }
    return map;
}

void printResultWithStdAlloc(map<string, size_t> map) {
    vector<pair<size_t, string>> words;
    for (pair<string, size_t> items : map) {
        words.push_back(pair<size_t, string>(items.second, items.first));
    }
    sort(words.rbegin(), words.rend());
    for (pair<size_t, string> items : words) {
        cout << items.second << ": " << items.first << endl;
    }
}

map<char*, size_t, CharCompare, MyAllocator<pair<char*, size_t>>> getMapWithMyAlloc(char* text) {
    map<char*, size_t, CharCompare, MyAllocator<pair<char*, size_t>>> mapWithAlloc;
    bool wordFound = false;
    for (size_t i = 0; text[i] != '\0'; i++) {
        if (text[i] > 0 && isalpha(text[i])) {
            if (!wordFound) {
                mapWithAlloc[text + i]++;
                wordFound = true;
            }
        }
        else
            wordFound = false;
    }
    return mapWithAlloc;
}

void printResultWithMyAlloc(map<char*, size_t, CharCompare, MyAllocator<pair<char*, size_t>>> mapWithAlloc) {
    multimap<size_t, char*> words;
    for (pair<char*, size_t> items : mapWithAlloc) {
        words.insert(pair<size_t, char*>(items.second, items.first));
    }
    multimap<size_t, char*>::reverse_iterator rit;
    for (rit = words.rbegin(); rit != words.rend(); rit++) {
        char *a = rit->second;
        while (*a > 0 && isalpha(*a)) {
            cout << (char)tolower(*a);
            a++;
        }
        cout << ": " << rit->first << endl;
    }
}

int main() {
    HANDLE hFile = CreateFile("test.txt", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    LARGE_INTEGER size;
    GetFileSizeEx(hFile, &size);
    cout << "File size: " << size.QuadPart << endl;

    unsigned long fileSize = size.QuadPart;
    char *text = new char[fileSize + 1];
    ReadFile(hFile, text, fileSize, NULL, NULL);
    text[fileSize] = '\0';
    CloseHandle(hFile);

    clock_t start;
    start = clock();
    auto map = getMapWithStdAlloc(text);
    start = clock() - start;
    cout << "Std allocator time: " << (double)start * 1000 / CLOCKS_PER_SEC << endl;
    printResultWithStdAlloc(map);

    start = clock();
    auto mapWithAlloc = getMapWithMyAlloc(text);
    start = clock() - start;
    cout << "My allocator time: " << (double)start * 1000 / CLOCKS_PER_SEC << endl;
    printResultWithMyAlloc(mapWithAlloc);

    delete[] text;
    return 0;
}