#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include <map>
#include <vector>
#include <algorithm>
#include <ctime>
#include "LinkedList.h"

using namespace std;

LONGLONG getFileSize(LPCWSTR filename) {
    LARGE_INTEGER fileSize;
    HANDLE hfile = CreateFile(
        filename,
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_SEQUENTIAL_SCAN,
        NULL);

    if (hfile == INVALID_HANDLE_VALUE) {
        cerr << "Couldn't open file temp.txt" << "\n";
    }

    if (!GetFileSizeEx(hfile, &fileSize)) {
        cerr << "Couldn't get size of file temp.txt" << "\n";
    }
    CloseHandle(hfile);
    LONGLONG size = fileSize.QuadPart;
    cout << "File size: " << size << "\n";
    return size;
}


void readFile(char* text, int size, LPCWSTR filename) {
    DWORD buf;
    HANDLE hfile = CreateFile(
        filename,
        GENERIC_ALL,
        FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    if (hfile == INVALID_HANDLE_VALUE)
    {
        cerr << "INVALID_HANDLE_VALUE";
        CloseHandle(hfile);
        return;
    }
    if (!ReadFile(hfile, text, size, &buf, NULL)) {
        cerr << "Couldn't read file temp.txt";
        return;
    }
    if (buf > 0) {
        text[buf] = '\0';
    }
    CloseHandle(hfile);
}

template <typename T, size_t size = 1024 * 1024 >
class Buffer {
public:
    void* ptr;
    size_t sp;
    size_t minOffset = 4000;

    Buffer() {
        ptr = malloc(size);
        sp = 0;
    }

    ~Buffer() {
        if (sp > minOffset)
            free(ptr);
    }

    T* allocate(size_t n) {
        if (n + sp > size) {
            return nullptr;
        }
        T* ret = (T*)((T*)ptr + sp);
        sp += n;
        return ret;
    }

    void deallocate(size_t n) {
        sp -= n;
    }
};

template<typename T>
struct customAllocator {
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;

    template< class U >
    struct rebind {
        typedef customAllocator<U> other;
    };

    customAllocator() noexcept {
        current = Buffer<T>();
        buffers = LinkedList<Buffer<T>>(current);
    };

    ~customAllocator() noexcept {
        while (buffers.getCount() != 0) {
            Buffer<T> bufferToDelete = buffers.popLeftElement();
            bufferToDelete.~Buffer();
        }
    };

    customAllocator(const customAllocator& other) noexcept {
        buffers = LinkedList<Buffer<T>>();
        current = Buffer<T>();
    };

    template< class U >
    customAllocator(const customAllocator<U>& other) noexcept
    {
        buffers = LinkedList<Buffer<T>>();
        current = Buffer<T>();
    }

    pointer allocate(size_t n)
    {
        pointer ret = current.allocate(n);
        if (ret == nullptr) {
            current = Buffer<T>();
            buffers.addElementRight(current);
            ret = current.allocate(n);
        }
        return ret;
    }

    void deallocate(pointer p, size_t n)
    {
        current.deallocate(n);
    }

private:
    LinkedList<Buffer<T>> buffers;
    Buffer<T> current;
};

size_t getLength(char* word) {
    size_t length = 0;
    while (true) {
        if ((int)word[length] <= -1 || (int)word[length] >= 255 || !isalpha(word[length])) {
            return length;
        }
        ++length;
    }
}

struct customCmpString {
    bool operator() (char *a, char *b) const {
        size_t La = getLength(a);
        size_t Lb = getLength(b);
        for (int i = 0; i < La && i < Lb; i++)
        {
            if (tolower(a[i]) < tolower(b[i]))
                return true;
            if (tolower(a[i]) > tolower(b[i]))
                return false;
        }
        if (La < Lb)
            return true;
        return false;
    }
};

bool cmp(const std::pair< string, size_t >& a, const std::pair< string, size_t >& b) {
    return a.second > b.second;
}

map<char*, size_t, customCmpString, customAllocator<char*>> parseBufferWithCustomAllocator(char* text, int size) {
    auto wordCount = map<char*, size_t, customCmpString, customAllocator<char*>>();
    int offset = 0;
    for (int i = 0; i <= size; i++) {
        if ((int)text[i] <= -1 || (int)text[i] >= 255 || !isalpha(text[i])) {
            offset = 0;
        }
        else {
            if (offset == 0) {
                char* refWord = text + i;
                wordCount[refWord] += 1;
            }
            ++offset;
        }
    }
    return wordCount;
}

map <string, size_t> parseBufferWithStandartAllocator(char* text, int size) {
    map <string, size_t> wordCount = map<string, size_t>();
    string currentWord = "";
    for (int i = 0; i <= size; i++) {
        if ((int)text[i] <= -1 || (int)text[i] >= 255 || !isalpha(text[i])) {
            if (currentWord == "") {
                continue;
            }
            if (wordCount.find(currentWord) == wordCount.end()) {
                wordCount[currentWord] = 0;
            }
            wordCount[currentWord] += 1;
            currentWord = "";
        }
        else {
            currentWord += tolower(text[i]);
        }
    }
    return wordCount;
}

void printResults(char* text, int size) {
    int startTime = clock();
    auto customWordCount = parseBufferWithCustomAllocator(text, size);
    int endTime = clock();
    vector< pair< char*, size_t > > customPairs(customWordCount.begin(), customWordCount.end());
    sort(customPairs.begin(), customPairs.end(), cmp);
    for (auto pair : customPairs) {
        size_t wordLength = getLength(pair.first);
        for (int i = 0; i < wordLength; ++i) {
            cout << pair.first[i];
        }
        cout << ' ' << pair.second << "\n";
    }
    double search_time = (endTime * 1.0 - startTime * 1.0) / 1000.0;
    cout << "Search time with custom allocator: " << search_time << " s" << "\n";
    startTime = clock();
    map <string, size_t> standartWordCount = parseBufferWithStandartAllocator(text, size);
    endTime = clock();
    vector< pair< string, size_t > > standartPairs(standartWordCount.begin(), standartWordCount.end());
    sort(standartPairs.begin(), standartPairs.end(), cmp);
    for (auto pair : standartPairs) {
        cout << pair.first << ' ' << pair.second << "\n";
    }
    search_time = (endTime * 1.0 - startTime * 1.0) / 1000.0;
    cout << "Search time with standart allocator: " << search_time << " s" << "\n";
}

int main() {
    LPCTSTR filename = L".\\temp.txt";
    int size = getFileSize(filename);
    char* text = new char[size + 1];
    readFile(text, size, filename);
    printResults(text, size);
    delete[] text;
    system("PAUSE");
    return 0;
}