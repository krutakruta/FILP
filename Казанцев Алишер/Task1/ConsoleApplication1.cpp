
#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <functional>
#include <time.h>
#include "CustomAllocator.h"

using namespace std;

enum Type {Custom, Default};

// Map comparator
struct MapComparator
{
	bool operator()(char const *a, char const *b) const
	{
		return strcmp(a, b) < 0;
	}
};
// End map comparator

// Set Comparator for sorting by value
typedef function<bool(pair<const char *, int>, pair<const char *, int>)> SetComparator;
SetComparator compFunctor =
[](pair<const char *, int> elem1, pair<const char *, int> elem2) {
	return elem1.second == elem2.second ?
		strcmp(elem1.first, elem2.first) < 0 : elem1.second < elem2.second;
};
// End Comparator for sorting

class Map {
public:
	map<const char*, int, MapComparator, CustomAllocator<pair<const char*, int>>> CustomMap;
	map<const char*, int> DefaultMap;
	clock_t ms_custom;
	clock_t ms_default;

	Type type;

	void set_type(Type type) {
		this->type = type;
	}

	void add_word(char* word) {
		if (type == Custom)
			CustomMap[word] += 1;
		else
			DefaultMap[word] += 1;
	}
};

void count_words(char* text, size_t size, Map* word_counter) {
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
				int position = i - word_length;
				for (int j = 0; j < word_length; j++)
					word[j] = tolower(text[position + j]);
				word[word_length] = 0;
				word_counter->add_word(word);
				word_length = 0;
			}
		}
	}
}


int main(int argc, const char * argv[]) {
    char path[1024];
    cout << "Please enter book path:" << endl;
    cin >> path;

	ifstream fs(path, ios::binary);
	
	if (!fs.is_open())
		throw "Cant open file";

	fs.seekg(0, ios::end);
	size_t Size = fs.tellg();
	fs.seekg(0);

	char* Text = new char[Size + 1];
	fs.read(Text, Size);
	Text[Size] = 0;
	Map* counter = new Map();
	clock_t start;

	// Time Map with default allocator
	counter->set_type(Default);
	start = clock();

	count_words(Text, Size, counter);

	start = clock() - start;
	counter->ms_default = start;
	// End

	// Time Map with custom allocator
	counter->set_type(Custom);
	start = clock();

	count_words(Text, Size, counter);

	start = clock() - start;
	counter->ms_custom = start;
	// End

	delete[] Text;
	fs.close();

	cout << "Results:\n" << "Custom: " << counter->ms_custom << "ms\n"
		 << "Default: " << counter->ms_default << "ms" << endl;

	cout << "\nDo you want to see sorted result?\nY\\N" << endl;
	char answer;
	cin >> answer;

	if (tolower(answer) == 'n')
		return 0;
	
	// Generate sorted Set
	set<pair<const char *, int>, SetComparator> setOfWords(
		counter->CustomMap.begin(), counter->CustomMap.end(), compFunctor);
	// End

	for (pair<const char *, int> element : setOfWords)
		cout << element.first << " - " << element.second << endl;

	system("PAUSE");

	return 0;
}
