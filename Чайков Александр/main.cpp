#include <iostream>
#include "windows.h"
#include "map"
#include <ctime>
#include "mallocator.h"
#include <string>
#include "algorithm"
#include <chrono>

using namespace std;
using namespace std::chrono;

char* const BOOK_FILE_NAME = "book.txt";
struct cmp_str
{
    bool operator()(const char* a, const char* b) const
    {
        while (isalpha(*a) && isalpha(*b)) {
            if (tolower(*a) != tolower(*b))
                return tolower(*a) < tolower(*b);
            a++;
            b++;
        }
        if (!isalpha(*a) && !isalpha(*b))
            return false;
        return isalpha(*a) != 0;
    }
};

using namespace std;

int getText(char* file_name, char** buffer);
void getWord(char** buffer);
map<char*, size_t, cmp_str, mallocator<pair<char* const, size_t>>> getWordsFreq(char* buffer);
void printWordByFirstChar(char* first);
map<string, size_t> defaultSolve(char* text);


int main() {
    char* text = nullptr;
    int codeGetText = getText(BOOK_FILE_NAME, &text);
    if (codeGetText)
        return codeGetText;
    auto start = high_resolution_clock::now();
    map<char*, size_t, cmp_str, mallocator<pair<char* const, size_t>>> wordsFreq = getWordsFreq(text);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "char* solution time : " << duration.count() << endl;
    for (auto & it : wordsFreq) {
        printWordByFirstChar(it.first);
        printf(" %d\n", it.second);
    }
    start = high_resolution_clock::now();
    map<string, size_t> counter = defaultSolve(text);
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    cout << "default solution time : " << duration.count() << endl;
    for (auto & it : counter) {
        cout << it.first << " ";
        printf(" %d\n", it.second);
    }

}

map<string, size_t> defaultSolve(char* text)
{
    map<string, size_t> strCounter;
    string currentWord  = "";
    char* textPtr = text;
    char* endPtr = text;
    while (true)
    {
        int len = 0;
        while(isalpha(*endPtr)){
            len++;
            endPtr++;
        }
        if(len){
            currentWord = string(textPtr, len);
            for_each(currentWord.begin(), currentWord.end(), [](char & c){
                c = tolower(c);
            });
            if(strCounter.find(currentWord) != strCounter.end())
                strCounter[currentWord]++;
            else
                strCounter.insert(pair<string, int>(currentWord, 1));
            textPtr = endPtr;
        }
        while(!isalpha(*textPtr) && *textPtr != '\0'){
            textPtr++;
            endPtr++;
        }
        if(*textPtr == '\0')
            break;
    }
    return strCounter;
}
map<char*, size_t, cmp_str, mallocator<pair<char* const, size_t>>> getWordsFreq(char* buffer) {
    map<char*, size_t, cmp_str, mallocator<pair<char* const, size_t>>> strCounter;
    bool isEnd = false;
    char* textPtr = buffer;
    while (true)
    {
        while (!isalpha(*textPtr)) {
            if (*textPtr == '\0') {
                isEnd = true;
                break;
            }
            textPtr++;
        }
        if (isEnd)
            break;
        if (strCounter.find(textPtr) != strCounter.end())
            strCounter[textPtr]++;
        else {
            strCounter.insert(pair<char*, int>(textPtr, 1));
        }
        getWord(&textPtr);
    }
    return strCounter;
}

int getText(char* const file_name, char** buffer) {
    LARGE_INTEGER fileSize;
    DWORD actualReadedBytes;

    HANDLE handleBookFile = CreateFile(file_name, GENERIC_READ,
                                       0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (handleBookFile == INVALID_HANDLE_VALUE)
        return 1;
    if (!GetFileSizeEx(handleBookFile, &fileSize))
        return 2;
    char* textBuffer = new char[fileSize.QuadPart + 1]();
    if (!ReadFile(handleBookFile, textBuffer, fileSize.QuadPart, &actualReadedBytes, NULL))
        return 3;
    *buffer = textBuffer;
    return 0;
}

void getWord(char** buffer) {
    int lenStr = 0;
    while (isalpha(**buffer)) {
        lenStr++;
        *(buffer) = *(buffer)+1;
    }
}
void printWordByFirstChar(char* first) {
    int len = 0;
    char* pointer = first;
    while (isalpha(*pointer)) {
        len++;
        pointer++;
    }
    printf("%.*s", len, first);
}
