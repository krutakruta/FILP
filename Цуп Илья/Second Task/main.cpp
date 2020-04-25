#include <iostream>
#include "allocator.h"
#include "buddy.h"
#include <windows.h>
#include <map>
#include <ctime>
#include <algorithm>
#include <vector>


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

    explicit File(char* filename) {
        hFile = CreateFile(filename,
                GENERIC_READ,
                FILE_SHARE_READ,
                nullptr,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                nullptr);

        if (hFile == INVALID_HANDLE_VALUE) {
            std::cout << "Can't open file";
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

    char* get_text() {
        if (text != nullptr)
            return text;
        text = new char[Size + 1];
        DWORD buff;

        ReadFile(hFile, text, Size, &buff, nullptr);

        text[buff] = '\x00';

        return text;
    }
};

using namespace std;

map<char*, size_t, MyComparator, BuddyAllocator<char*>> map_with_custom_allocator(char* text) {
    auto word_counter = map<char*, size_t, MyComparator, BuddyAllocator<char*>>();
    bool word_begins = true;
    while(*text != '\x00') {
        if (isalpha(*text)){
            if (word_begins){
                text[0] = tolower(text[0]);
                word_counter[text] += 1;
                word_begins = false;
            }
        }
        else
            word_begins = true;
        ++text;
    }
    return word_counter;
}

map<string, size_t> map_with_system_allocator(char* text) {
    map<string, size_t> word_counter = map<string, size_t>();
    string current_word;
    while (*text != '\x00') {
        if (isalpha(*text))
            current_word += tolower(*text);
        else {
            if (!current_word.empty()) {
                word_counter[current_word] += 1;
                current_word = "";
            }
        }
        text++;
    }
    return word_counter;
}

template <typename T>
bool cmp(pair<T, size_t> first, pair<T, size_t> second) {
    return first.second > second.second;
}


void run_tests(char* text, DWORD size) {
    uint start_time = clock();
    auto custom_allocator_mapped = map_with_custom_allocator(text);
    uint time_with_custom_allocator = clock() - start_time;
    start_time = clock();
    auto system_allocator_mapped = map_with_system_allocator(text);
    uint time_with_system_allocator = clock() - start_time;

    vector<pair<char*, size_t>> custom_sorted;
    for (auto &it : custom_allocator_mapped)
        custom_sorted.emplace_back(it);

    sort(custom_sorted.begin(), custom_sorted.end(), cmp<char*>);

    vector<pair<string, size_t>> system_sorted;
    for (auto &it : system_allocator_mapped)
        system_sorted.emplace_back(it);

    sort(system_sorted.begin(), system_sorted.end(), cmp<string>);

    cout << "File allocationSize: " << size << " bytes\n";
    cout << "\tMY ALLOCATOR\t\t\tSYSTEM ALLOCATOR\n";
    cout << "time:\t" << time_with_custom_allocator << "ms\t\t\t\t" << time_with_system_allocator << "ms\n";
    cout << "words (word : frequency):\n";
    auto custom_iterator = custom_sorted.begin();
    auto system_iterator = system_sorted.begin();
    for (; custom_iterator != custom_sorted.end() && system_iterator != system_sorted.end();
    ++custom_iterator, ++system_iterator) {
        cout << "\t";
        char* letter = custom_iterator->first;
        while (isalpha(*letter))
            cout << *(letter++);
        cout << " : " << custom_iterator->second << "\t\t\t\t" << system_iterator->first << " : " << system_iterator->second << endl;
    }
}


int main() {
    auto file = File((char*)"Data/test.txt");
    auto text = file.get_text();
    run_tests(text, file.Size);
    file.Close();
}