#include "pch.h"
#include <windows.h>
#include <iostream>
#include <tchar.h>
#include <map>
#include <time.h>
#include <vector>
#include <cstdlib>
#include <string>
#include <iterator>
#include <algorithm>
using namespace std;
const int sizeBuf = 1024 * 1024;

template <class T>
struct Allocator
{
	typedef T value_type;
	vector<void*> buffers;
	size_t size = 0;
	T* ptr = NULL;

	//template <class U> 
	//Allocator(const Allocator <U>&) noexcept {}
	Allocator& operator=(const Allocator& x) = default;

	T* allocate(size_t n) {
		if (size < n)
		{
			ptr = static_cast<T*>(malloc(sizeBuf));
			buffers.push_back(ptr);
			size = sizeBuf;
		}
		size -= n * sizeof(T);
		return ptr++;
	}

	void deallocate(T* p, size_t n) {}
};

struct Comparator
{
	bool operator()(const char *a, const char *b) const
	{
		while (isalpha(*a) && isalpha(*b) && *a >= 0 && *b >= 0)
		{
			auto l_a = tolower(*a);
			auto l_b = tolower(*b);
			if (l_a < l_b)
				return true;
			if (l_a > l_b)
				return false;
			a++;
			b++;
		}
		if (*a > 0 && *b > 0)
			return isalpha(*a) < isalpha(*b);
		return false;
	}
};

template <class T>
bool cmp(const pair<T, size_t>& a, const pair<T, size_t>& b)
{
	return a.second < b.second;
}

void AnalyzeSimpleAllocator(char* text)
{
	clock_t start;
	start = clock();
	map<string, size_t> counter;

	bool isFirst = true;
	char* token = NULL;
	while (isFirst or token != NULL)
	{
		token = strtok(NULL, " ,.:;!?\r\n\t");
		counter[token]++;
		isFirst = false;
	}

	cout << "Time standart Allocator: " << clock() - start << "sec" << endl;
	getResult(counter);
}

void getResult(map<string, size_t> counter)
{
	vector<pair<string, size_t>> result;
	for (auto element : counter)
	{
		result.push_back(element);
	}
	sort(result.begin(), result.end(), cmp<string>);

	int size = result.size();
	for (auto i = size - 1; i > size; i--)
	{
		cout << result[i].first << " -- " << result[i].second << "\n";
	}
}

void AnalyzeMyAllocator(char* text)
{
	clock_t start;
	start = clock();
	vector<pair<char*, size_t>> result;
	map<char*, size_t, Comparator, Allocator<pair<char*, size_t> >> counter;

	bool isFirst = true;
	char* token = NULL;
	while (isFirst or token != NULL)
	{
		token = strtok(NULL, " ,.:;!?\r\n\t");
		counter[token]++;
		isFirst = false;
	}
	cout << "Time custom Allocator: " << clock() - start << "sec" << "\n";

	for (auto element : counter)
	{
		result.push_back(element);
	}
	sort(result.begin(), result.end(), cmp<char*>);

	int size = result.size();
	for (auto i = size - 1; i > size; i--)
	{
		cout << result[i].first << " -- " << result[i].second << "\n";
	}
}

int main(int argc, char *argv[])
{
	HANDLE hFile = CreateFile(
		L"..\book.txt",
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	LARGE_INTEGER size;
	GetFileSizeEx(hFile, &size);
	char* text = new char[size.QuadPart + 1];
	ReadFile(hFile, text, size.QuadPart, NULL, NULL);
	text[size.QuadPart] = '\0';
	CloseHandle(hFile);

	AnalyzeSimpleAllocator(text);
	AnalyzeMyAllocator(text);
}