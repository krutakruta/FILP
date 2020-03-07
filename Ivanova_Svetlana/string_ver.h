#include <iostream>
#include <map>
#include <fstream>
using namespace std;

multimap<int, std::string, greater<int>> invert_map(std::map<std::string, int> &mymap)
{
    multimap<int, std::string, greater<int>> multiMap;

    map<string, int> :: iterator it;
    for (it=mymap.begin(); it!=mymap.end(); it++)
    {
        multiMap.insert(make_pair(it->second, it->first));
    }

    return multiMap;
}

std::string read_string_from_file(const char *&filename) {
    std::ifstream t(filename);
    std::string str;

    t.seekg(0, std::ios::end);
    str.reserve(t.tellg());
    t.seekg(0, std::ios::beg);

    str.assign((std::istreambuf_iterator<char>(t)),
               std::istreambuf_iterator<char>());
    return str;
}

void print_map(std::multimap<int, std::string, greater<int>> &my_map)
{
    for (auto& s : my_map)
    {
        cout << s.second << " " << s.first << endl;
    }
}

auto count_words_in_str(std::string text)
{
    std::map <std::string, int> word_frequency;
    string word="";
    char c;
    for (int i=0; i <= text.length(); i++){
        if (isalpha((int)text[i]) && i < text.length()){
            c = text[i];
            word = word + c;
        }
        else if (word.length()>0) {
            if (word_frequency.count(word) == 0) {
                word_frequency[word] = 1;
                word = "";
            } else {
                word_frequency[word]++;
                word = "";
            }
        }

    }
    return word_frequency;
}

void str_ver(const char *&filename) {
    std::string text;
    text = read_string_from_file(filename);

    clock_t begin = clock();
    auto words = count_words_in_str(text);
    auto inverted = invert_map(words);
    clock_t end = clock();
    cout << "_______________________________________________\n";
    cout << "time of str_ver exec " << double(end - begin) / (double)CLOCKS_PER_SEC<<"\n";
    cout << "_______________________________________________\n";
    print_map(inverted);
}