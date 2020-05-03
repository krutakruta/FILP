#include <fstream>
#include <iostream>
#include <map>
#include <string.h>
#include <cstring>
#include "allocator.h"
#include <ctime>


using namespace std;

struct cmp_str {
    //from libc with small changes
    bool operator()(char const *a, char const *b) const {
        const unsigned char *s1 = (const unsigned char *) a;
        const unsigned char *s2 = (const unsigned char *) b;
        unsigned char c1, c2;
        do
        {
            c1 = (unsigned char) *s1++;
            c2 = (unsigned char) *s2++;
            if (!(isalpha(c1)))
                return false;
            if (!(isalpha(c2)))
                return true;

        }
        while (c1 == c2);
        return c1 - c2<0;
    }
};

multimap<int, const char *, greater<int>> invert_map(std::map<const char *, int, cmp_str, mmap_allocator<char*>> &mymap)
{
    multimap<int, const char *, greater<int>> multiMap;

    map<const char *, int> :: iterator it;
    for (it=mymap.begin(); it!=mymap.end(); it++)
    {
        multiMap.insert(make_pair(it->second, it->first));
    }

    return multiMap;
}

using Map = std::map<const char *, int, cmp_str, mmap_allocator<char*>>;

std::ifstream::pos_type filesize(const char *&filename) {
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

char *read_char_from_file(const char *&filename) {
    int size = filesize(filename);
    char *text;

    ifstream file(filename, ios::in | ios::binary);
    // ВЫДЕЛЕНИЕ ПАМЯТИ
    text = new char[size + 1];
    file.seekg(0, ios::beg);
    file.read(text, size);
    file.close();
    return text;
}


void print_origin_map(Map &my_map) {
    for (auto &s : my_map) {
        int i = 0;
        while (isalpha(s.first[i]))
        {
            cout << s.first[i];
            i++;
        }
        cout << " " << s.second << endl;
    }
}

void print_map(multimap<int, const char *, greater<int>> &my_map) {
    for (auto &s : my_map) {
        int i = 0;
        while (isalpha(s.second[i]))
        {
            cout << s.second[i];
            i++;
        }
        cout << " " << s.first << endl;
    }
}


auto count_words_in_char(const char *text) {
    Map word_frequency;
    int start_pointer = 0;
    int text_size = strlen(text);
    int c;
    for (int i = 0; i <= text_size; i++) {
        c = (int) text[i];
        if (isalpha(c) && i < text_size) {
            continue;
        } else {
            if (i == start_pointer) {
                start_pointer++;
            } else {
                const char *word = &text[start_pointer];
                if (word_frequency.count(word) == 0) {
                    word_frequency[word] = 1;
                } else {
                    word_frequency[word]++;
                }
                start_pointer = i + 1;

            }
        }

    }
    return word_frequency;
}


void char_ver(const char *&filename) {
    char *text;
    text = read_char_from_file(filename);

    clock_t begin = clock();
    auto word_freq = count_words_in_char(text);
    auto inverted = invert_map(word_freq);
    clock_t end = clock();
    cout << "_______________________________________________\n";
    cout << "time of char_ver exec " << double(end - begin) / (double)CLOCKS_PER_SEC <<"\n";
    cout << "_______________________________________________\n";
    print_map(inverted);

}
