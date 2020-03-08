
#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <algorithm>
#include <functional>
#include "CustomAllocator.h"

using namespace std;

enum Type {Custom, Default};

struct CompareStrings
{
	bool operator()(char const *a, char const *b) const
	{
		return strcmp(a, b) < 0;
	}
};

class MyMap {
public:
	map<const char*, int, CompareStrings, CustomAllocator<pair<const char*, int>>> CustomMap;
	map<const char*, int, CompareStrings> DefaultMap;
	Type type;

	MyMap(Type type) {
		this->type = type;
	}


	void add_word(char* word) {
		if (type == Custom)
			CustomMap[word] += 1;
		else
			DefaultMap[word] += 1;
	}
};

MyMap* count_words(char* text, size_t size, Type type) {
	MyMap* word_counter = new MyMap(type);

	int word_length = 0;
	char* word = nullptr;

	for (int i = 0; i < size && text[i] != '\0'; i++) {
		if (text[i] < 0)
			continue;

		if (isalpha(text[i]))
			word_length += 1;
		else {
			if (word_length != 0) {
				word = new char[word_length + 1];
				int counter = 0;
				int position = i - word_length;
				for (int j = 0; j < word_length; j++)
					word[j] = tolower(text[position + j]);
				word[word_length] = 0;
				word_counter->add_word(word);
				word_length = 0;
			}
		}
	}

	return word_counter;
}


int main(int argc, const char * argv[]) {
	//    ifstream fs("/Users/aliser/Desktop/greatestGameEver/greatestGameEver/text.txt", ios::binary);

    char path[1024];
    cout << "Please enter book path:" << endl;
    cin >> path;

	//  Убрать
	//string path = "C:\\Users\\Алишер\\Documents\\Visual Studio 2017\\Projects\\ConsoleApplication1\\book.txt";
	//

	ifstream fs(path, ios::binary);

	fs.seekg(0, ios::end);
	size_t Size = fs.tellg();
	fs.seekg(0);

	char* Text = new char[Size + 1];
	fs.read(Text, Size);
	Text[Size] = 0;

	MyMap* counter = count_words(Text, Size, Default);

	for (map<char const*, int>::iterator p = counter->DefaultMap.begin(); p != counter->DefaultMap.end(); ++p) {
		cout << p->first << ": " << p->second << endl;
	}

	for (map<char const*, int>::iterator p = counter->CustomMap.begin(); p != counter->CustomMap.end(); ++p) {
		cout << p->first << ": " << p->second << endl;
	}

	delete[] Text;
	fs.close();

	system("PAUSE");

	return 0;
}
