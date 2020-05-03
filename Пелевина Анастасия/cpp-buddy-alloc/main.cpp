#include <iostream>
#include <fstream>
#include <map>

#include "MyAlloc.h"
#include "utils.h"

using namespace std;

int main() {
    const char* filename = "./book1.txt";

    ifstream file(filename, ifstream::ate | ifstream::binary);

    if (!file) {
        cout << "File wasn't opened!" << endl;
        return -1;
    }

    int file_size = file.tellg();

    char* text = new char[file_size + 1];

    file.seekg(0);
    file.read(text, file_size);
    file.close();

    const char* const_text = text;
    int text_size = strlen(text);
    map<string, size_t> counter;
    string word;

    int start_time = clock();

    for (int i = 0; i < text_size; i++) {
        if (!isalpha((int) const_text[i])) {
            if (word.length() == 0) {
                continue;
            }
            if (counter.count(word) == 0) {
                counter[word] = 1;
            } else {
                counter[word]++;
            }
            word = "";
        } else {
            word += text[i];
        }
    }

    int system_time = clock() - start_time;

    auto new_counter = map<char*, size_t, MyCompare, MyAlloc<char*> >();
    char* new_word;
    int start = 0;

    start_time = clock();

    for (int i = 0; i < text_size; i++) {
        if (!isalpha((int) const_text[i])) {
            if (i == start) {
                start++;
            }
            new_word = new char[i - start + 1];

            for (int j = 0; j < i - start; j++) {
                new_word[j] = const_text[start + j];
            }
            if (strlen(new_word) == 0) {
                continue;
            }
            start = i + 1;
            if (new_counter.count(const_cast<char*>(new_word)) == 0) {
                new_counter[new_word] = 1;
            } else {
                new_counter[new_word]++;
            }
        }
    }

    int my_time =  clock() - start_time;

    auto it = new_counter.begin();
    for (int i = 0; it != new_counter.end(); it++, i++) {
        cout << i << ") word " << it->first << ", count " << it->second << endl;
    }

    cout << endl;

    auto it2 = counter.begin();
    for (int i = 0; it2 != counter.end(); it2++, i++) {
        cout << i << ") word " << it2->first << ", count " << it2->second << endl;
    }

    cout << "System allocator time: " << system_time << " ms" << endl;

    cout << "Custom allocator time: "  << my_time << " ms" << endl;

    return 0;
}