#include <iostream>
#include <map>
#include <conio.h>
#include <vector>
#include <Windows.h>
#include <algorithm>
#include "stdlib.h" 
#include <time.h>  
#include <string>

#include "Mallocator.h"

using namespace std;


#define FILENAME "big.txt"
#define DELIM " ,._\t\n;:\r\"?-"


struct cmpByChar
{
	bool operator()(const char* a, const char* b) const
	{
		return strcmp(a, b) == -1;
	}
};

char* GetText()
{
	auto hFile = CreateFile(FILENAME, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	LARGE_INTEGER lFileSize;
	GetFileSizeEx(hFile, &lFileSize);
	char* ReadBuffer = new char[lFileSize.QuadPart + 1];

	cout << "File size: " << lFileSize.QuadPart << endl;

	DWORD nRead;
	if (FALSE == ReadFile(hFile, ReadBuffer, lFileSize.QuadPart, &nRead, NULL))
	{
		printf("Terminal failure: Unable to read from file.\n GetLastError=%08x\n", GetLastError());
		CloseHandle(hFile);

		delete ReadBuffer;
		return nullptr;
	}
	CloseHandle(hFile);
	
	return ReadBuffer;
}

void FrequencyWords_withCustomAlloc(bool print=false)
{
	auto ReadBuffer = GetText();

	unsigned int start_time = clock(); // начальное время

	char delim[] = DELIM;
	char *next_token1 = NULL;
	map<char*, size_t, cmpByChar, mallocator<pair<char*, size_t>>> words;

	char *ptr = strtok_s(ReadBuffer, delim, &next_token1);
	while (ptr != NULL)
	{
		_strlwr_s(ptr, strlen(ptr) + 1);
		words[ptr]++;
		ptr = strtok_s(NULL, delim, &next_token1);
	}

	unsigned int second_time = clock(); // конечное время
	cout << "Длительность создания map: " << second_time - start_time << endl;

	vector<pair<char*, int>>vec;
	for (auto x = words.begin(); x != words.end(); x++)
		vec.push_back(*x);
	sort(vec.begin(), vec.end(), [](pair<char*, int>elem1, pair<char*, int>elem2) {return elem1.second > elem2.second; });

	if (print)
	{
		for (auto x : vec)
			cout << x.first <<" "<< x.second << endl;
	}
	
	unsigned int end_time = clock(); // конечное время
	cout << "Длительность процесса: " << end_time - start_time << endl;

	delete ReadBuffer;
}


void FrequencyWords_withStandardAlloc(bool print = false)
{
	auto ReadBuffer = GetText();

	unsigned int start_time = clock(); // начальное время

	char delim[] = DELIM;
	char *next_token1 = NULL;
	map<string, size_t> words;

	char *ptr = strtok_s(ReadBuffer, delim, &next_token1);
	while (ptr != NULL)
	{
		_strlwr_s(ptr, strlen(ptr) + 1);
		words[ptr]++;
		ptr = strtok_s(NULL, delim, &next_token1);
	}

	unsigned int second_time = clock(); // конечное время
	cout << "Длительность создания map: " << second_time - start_time << endl;

	vector<pair<string, int>>vec;
	for (auto x = words.begin(); x != words.end(); x++)
		vec.push_back(*x);
	sort(vec.begin(), vec.end(), [](pair<string, int>elem1, pair<string, int>elem2) {return elem1.second > elem2.second; });

	if (print)
	{
		for (auto x : vec)
			cout << x.first << " " << x.second << endl;
	}

	unsigned int end_time = clock(); // конечное время
	cout << "Длительность процесса: " << end_time - start_time;

	delete ReadBuffer;
}

int main()
{
	setlocale(LC_ALL, "Russian");

	cout << "Frequecy with custom algorythm:\n";
	FrequencyWords_withCustomAlloc();

	cout << "Frequecy with standard algorythm:\n";
	FrequencyWords_withStandardAlloc();

	_getch();

	return 0;
}