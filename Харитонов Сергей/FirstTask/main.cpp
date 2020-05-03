#include <iostream>
#include "windows.h"
#include <map>
#include <ctime>
#include <algorithm>
#include <vector>

template <class T>
struct MyAllocator {
    typedef T value_type;
    typedef T* pointer;
    typedef size_t size_type;

    MyAllocator() {
        ptr = malloc(1024 * 1024);
        lastP = 0;
    }

    pointer allocate(size_t size) {
        pointer res = ((pointer)ptr) + lastP;
        lastP += size;

        return res;
    }

    void deallocate(pointer p, size_t size) {
        lastP -= size;
    }

    ~MyAllocator() {
        free(ptr);
    }
private:
    void* ptr;
    size_t lastP;
};

struct MyComparator {
    bool operator() (char* a, char* b) const {
        while (isalpha(*a)) {
            char lowA = tolower(*a);
            char lowB = tolower(*b);
            if (lowA < lowB)
                return true;
            if (lowA > lowB)
                return false;

            a++; b++;
        }

        return isalpha(*b);
    }
};

struct File {
private:
    HANDLE hFile;
    char* filename;
    char* text = nullptr;
    DWORD GetSize() {
        LARGE_INTEGER size;
        bool isOk = GetFileSizeEx(hFile, &size);

        if (isOk) {
            return size.QuadPart;
        }

        exit(-1);
    }
public:
    DWORD Size;
    File(char* filename) {
        this->filename = filename;
        hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

        if (hFile == INVALID_HANDLE_VALUE) {
            std::cout << "Can not open file";
            exit(-1);
        }

        Size = GetSize();
    }

    ~File() {
        Close();
    }

    void Close() {
        CloseHandle(hFile);
    }

    char* GetText() {
        if (text != nullptr)
            return text;
        text = new char[Size + 1];
        DWORD buff;

        ReadFile(hFile, text, Size, &buff, NULL);

        text[buff] = '\x00';

        return text;
    }
};

using namespace std;

map<char*, size_t, MyComparator, MyAllocator<char*>> parseUsingMyAllocator(char* text) {
    auto wordsCounter = map<char*, size_t, MyComparator, MyAllocator<char*>>();
    bool isWordBegin = true;

    while(*text != '\x00') {
        if (isalpha(*text)){
            if (isWordBegin){
                wordsCounter[text] += 1;
                isWordBegin = false;
            }
        }
        else
            isWordBegin = true;

        ++text;
    }

    return wordsCounter;
}

map<string, size_t> parseUsingSystemAllocator(char* text) {
    map<string, size_t> wordsCounter = map<string, size_t>();
    string currWord;
    while (*text != '\x00') {
        if (isalpha(*text))
            currWord += tolower(*text);
        else {
            if (!currWord.empty()) {
                wordsCounter[currWord] += 1;
                currWord = "";
            }
        }

        text++;
    }

    return wordsCounter;
}

template <typename T>
bool cmp(pair<T, size_t> first, pair<T, size_t> second) {
    return first.second > second.second;
}


void analyzeAndGetResults(char* text, DWORD size) {
    int startTime = clock();
    auto firstMap = parseUsingMyAllocator(text);
    int timeUsingMyAllocator = clock() - startTime;

    startTime = clock();
    auto secondMap = parseUsingSystemAllocator(text);
    int timeUsingSystemAllocator = clock() - startTime;

    vector<pair<char*, size_t>> sortedFirst;
    for (auto &it : firstMap)
        sortedFirst.push_back(it);

    sort(sortedFirst.begin(), sortedFirst.end(), cmp<char*>);

    vector<pair<string, size_t>> sortedSecond;
    for (auto &it : secondMap)
        sortedSecond.push_back(it);

    sort(sortedSecond.begin(), sortedSecond.end(), cmp<string>);

    cout << "File size: " << size << " bytes\n";
    cout << "\tMY ALLOCATOR\t\t\tSYSTEM ALLOCATOR\n";
    cout << "time:\t" << timeUsingMyAllocator << "ms\t\t\t\t" << timeUsingSystemAllocator << "ms\n";
    cout << "words (word : frequency):\n";
    auto it1 = sortedFirst.begin();
    auto it2 = sortedSecond.begin();
    for (; it1 != sortedFirst.end() && it2 != sortedSecond.end(); ++it1, ++it2) {
        cout << "\t";
        char* letter = it1->first;
        while (isalpha(*letter))
            cout << *(letter++);
        cout << " : " << it1->second << "\t\t\t\t" << it2->first << " : " << it2->second << endl;
    }
}


int main() {
    auto file = File((char*)"./test.txt");
    auto text = file.GetText();
    analyzeAndGetResults(text, file.Size);
    file.Close();
}
