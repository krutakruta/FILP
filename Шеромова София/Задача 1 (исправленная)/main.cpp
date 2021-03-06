#include <iostream>
#include <windows.h>
#include <map>
#include <vector>
#include <algorithm>
#include <ctime>
#include "CustomAllocator.h"

using namespace std;

struct CustomStringComparator {
    bool operator()(char *a, char *b) const {
        while (isalpha(*a)) {
            char aLow = (char) tolower(*a);
            char bLow = (char) tolower(*b);
            if (aLow < bLow)
                return true;
            if (aLow > bLow)
                return false;

            a++;
            b++;
        }

        return isalpha(*b);
    }
};

bool comparator(const pair<string, size_t> &a, const pair<string, size_t> &b) {
    return a.second > b.second;
}

bool GetFileSize(const TCHAR filename[], unsigned *size) {
    HANDLE hFile = CreateFile(filename, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

    if (INVALID_HANDLE_VALUE == hFile)
        return false;

    LARGE_INTEGER liSize;
    if (!GetFileSizeEx(hFile, &liSize))
        return false;

    CloseHandle(hFile);

    *size = liSize.QuadPart;
    return true;
}

bool ReadTextFile(const TCHAR filename[], char *text, unsigned size) {
    HANDLE hFile = CreateFile(filename, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

    if (INVALID_HANDLE_VALUE == hFile)
        return false;

    DWORD dwTemp = 0;
    if (!ReadFile(hFile, text, size, &dwTemp, nullptr)) {
        CloseHandle(hFile);
        return false;
    }

    if (size != dwTemp) {
        CloseHandle(hFile);
        return false;
    }

    text[dwTemp] = '\0';
    CloseHandle(hFile);

    return true;
}

map<char *, size_t, CustomStringComparator, CustomAllocator<char *>> parseWithCustomAllocator(
        char *text, unsigned size) {
    auto wordCounter = map<char *, size_t, CustomStringComparator, CustomAllocator<char *>>();
    int offset = 0;
    for (int i = 0; i <= size; i++) {
        if (!isalpha(text[i])) {
            offset = 0;
            continue;
        }
        if (offset == 0) {
            char *word = text + i;
            wordCounter[word] += 1;
        }
        offset++;
    }
    return wordCounter;
}

map<string, size_t> parseWithSystemAllocator(char *text) {
    map<string, size_t> wordsCounter = map<string, size_t>();
    string currentWord;
    while (*text != '\0') {
        if (isalpha(*text))
            currentWord += tolower(*text);
        else if (!currentWord.empty()) {
            wordsCounter[currentWord] += 1;
            currentWord = "";
        }
        text++;
    }

    return wordsCounter;
}

size_t getWordLength(char *word) {
    size_t length = 0;
    while (isalpha(word[length]))
        length++;

    return length;
}

void
printCustomResult(map<char *, size_t, CustomStringComparator, CustomAllocator<char *>> wordCount, double parseTime) {
    cout << "Parse time with custom allocator: " << parseTime << "s" << endl << "Result: " << endl;

    vector<pair<char *, size_t>> pairs(wordCount.begin(), wordCount.end());
    sort(pairs.begin(), pairs.end(), comparator);

    for (auto pair : pairs) {
        size_t wordLength = getWordLength(pair.first);
        cout << "\"";
        for (int i = 0; i < wordLength; ++i) {
            cout << pair.first[i];
        }
        cout << "\" : " << pair.second << ", ";
    }
    cout << endl;
}

void printSystemResult(map<string, size_t> wordCount, double parseTime) {
    cout << "Parse time with system allocator: " << parseTime << "s" << endl << "Result: " << endl;

    vector<pair<string, size_t>> pairs(wordCount.begin(), wordCount.end());
    sort(pairs.begin(), pairs.end(), comparator);

    for (const auto &pair : pairs) {
        cout << "\"" << pair.first << "\" : " << pair.second << ", ";
    }
    cout << endl;
}

int main() {
    const TCHAR filename[] = "input.txt";
    unsigned size = 0;

    if (!GetFileSize(filename, &size)) {
        cerr << "Failed to determine file size." << endl;
        return 1;
    }
    cout << "File size: " << size << endl;
    char *text = new char[size + 1];

    if (!ReadTextFile(filename, text, size)) {
        cerr << "Failed to read file." << endl;
        return 2;
    }

    double startTime = 0, endTime = 0, customTime = 0, systemTime = 0;

    for (int i = 0; i < 10; i++) {
        startTime = clock();
        parseWithCustomAllocator(text, size);
        endTime = clock();
        customTime += (endTime - startTime);
    }

    for (int i = 0; i < 10; i++) {
        startTime = clock();
        parseWithSystemAllocator(text);
        endTime = clock();
        systemTime += (endTime - startTime);
    }

    auto resultWithCustomAllocator = parseWithCustomAllocator(text, size);
    auto resultWithSystemAllocator = parseWithSystemAllocator(text);

    printCustomResult(resultWithCustomAllocator, customTime / 10000);
    printSystemResult(resultWithSystemAllocator, systemTime / 10000);

    delete[] text;
    return 0;
}
