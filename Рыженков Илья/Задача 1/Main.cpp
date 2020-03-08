#include <windows.h>
#include <iostream>
#include <cctype>
#include <map>
#include <algorithm>
#include <vector>
#include <memory>
#include <time.h>
#include "manager.h"

using namespace std;

//Comparator for char*
struct wordComparator
{
	bool operator()( char* a, char* b) const {
		int i = 0;
		while (*(a + i) > 0 && *(b + i) > 0 && isalpha(*(a + i)) && isalpha(*(b + i)))
		{
			if (*(a + i) == *(b + i))
			{
				i++;
				continue;
			}
			return *(a + i) < *(b + i);
		}
		return *(b + i) > 0 && isalpha(*(b + i));
	}
};

//Finds the word`s end
void find_word(char* text, int& word_start)
{
	int i = word_start;
	while (text[i] > 0)
	{
		if (isalpha(text[i]))
		{
			i++;
		}
		else
		{
			word_start = i;
			return;
		}
	}
	word_start = i;
}

//Functions that parsing text
map<char*, int, wordComparator, memoryManager< pair<char*, int> > > find_words_custom_alloc(char* text)
{
	int word_start_position = 0;
	int i = 0;
	map<char*, int, wordComparator, memoryManager<pair<char*, int>>> dict;
	while (text[i] != 0)
	{
		while (!isalpha(text[i]) && text[i] != 0)
		{
			i++;
		}
		word_start_position = i;
		find_word(text, i);
		char* word = text + word_start_position;
		auto it = dict.find(word);
		if (it != dict.end())
		{
			it->second++;
		}
		else
		{
			dict.emplace(word, 1);
		}
	}
	return dict;
}

map<char*, int, wordComparator> find_words_default_alloc(char* text)
{
	int word_start_position = 0;
	int i = 0;
	map<char*, int, wordComparator> dict;
	while (text[i] != 0)
	{
		if (text[i] < 0)
		{
			i++;
			continue;
		}
		while (text[i] > 0 && !isalpha(text[i]))
		{
			i++;
		}
		word_start_position = i;
		find_word(text, i);
		char* word = text + word_start_position;
		auto it = dict.find(word);
		if (it != dict.end())
		{
			it->second++;
		}
		else
		{
			dict.emplace(word, 1);
		}
	}
	return dict;
}

//Function for comparing pairs to sort result
bool cmpPairs(const pair<string, int> &a, const pair<string, int> &b)
{
	return a.second > b.second;
}

//Function for printing result
void print_result(map<char*, int, wordComparator> &res)
{
	vector<pair<char*, int>> list;
	for (auto it = res.begin(); it != res.end(); it++)
		list.push_back(*it);
	sort(list.begin(), list.end(), cmpPairs);
	for (auto it = list.begin(); it != list.end(); it++)
	{
		int s = 0;
		while (it->first[s] > 0 && isalpha(it->first[s]))
		{
			cout << it->first[s];
			s++;
		}
		cout << ": " << it->second << endl;
	}
}

//Two functions for elapsing time
void elapse_time_custom_alloc(char* Text)
{
	unsigned int start = clock();
	map<char*, int, wordComparator, memoryManager< pair<char*, int> > > res = find_words_custom_alloc(Text);
	unsigned int end = clock();
	cout << "Work time with custom allocator is " << end - start << endl << endl;
}

void elapse_time_default_alloc(char* Text, bool print = false)
{
	unsigned int start = clock();
	map<char*, int, wordComparator> res = find_words_default_alloc(Text);
	unsigned int end = clock();
	cout << "Work time with default allocator is " << end - start << endl;
	if (print)
		print_result(res);
}

int main()
{
	#define FILENAME "text.txt"
	HANDLE hFile;
	hFile = CreateFile(
		FILENAME, GENERIC_READ, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("Reading error");
		getchar();
		return 1;
	}
	int size = GetFileSize(hFile, NULL);
	char *Text = new char[size + 1];
	if (!ReadFile(hFile, Text, size, 0, NULL))
	{
		printf_s("Reading error");
		return 1;
	}
	Text[size] = 0;
	CloseHandle(hFile);
	elapse_time_custom_alloc(Text);
	//elapse_time_default_alloc(Text, true);
	system("pause");
	return 0;
}